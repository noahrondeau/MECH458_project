

/* ====== LIBRARY INCLUDES ====== */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <stdio.h>

/* ====== USER INCLUDES ======*/

#include "config.h"
#include "LedBank.h"
#include "SysClk.h"
#include "DcMotor.h"
#include "ADC.h"
#include "FerroSensor.h"
#include "HallSensor.h"
#include "OpticalSensor.h"
#include "Timer.h"
#include "PushButton.h"
#include "Queue.h"
#include "Tray.h"
#include "Filter.h"
#include "time.h"
#include "uart.h"

/* ====== FUNCTION PROTOTYPES ====== */
void Initialize();
ItemClass Classify(QueueElement elem);

void Welcome();
void UartDisplay();
void TriggerPauseManual();

/* ====== GLOBAL SYSTEM RESOURCES ====== */

// system resource handles, includes both internal and external periphs
// all these types are typedef'd as volatile, don't need to repeat here
DcMotor			belt;
ADCHandle		adc;
FerroSensor		ferro;
OpticalSensor	s1_optic;
OpticalSensor	s2_optic;
OpticalSensor	exit_optic;
PushButton		pauseButton;
PushButton		rampDownButton;
Tray			tray;

// State variable
FiniteStateMachine fsmState = {
	.state = RUN_STATE,
	.rampDownInitFlag = false,
	.rampDownEndFlag = false,
};

// queue for items *already* processed
Queue* readyQueue;
// queue for items *being* processed
Queue* processQueue;

// sensor stage 2 stats singleton struct
// to hold temp values used for sensor stage 2
volatile struct {
	uint16_t minReflectivity;
	uint16_t sampleCount;
	bool adcContinueConversions;
	bool badItemFlag;
} Stage2 = {
	.minReflectivity = LARGEST_UINT16_T,
	.sampleCount = 0,
	.adcContinueConversions = false,
	.badItemFlag = false,
};

// stage 3 flags
volatile struct {
	bool itemReady;
} Stage3 = {
	.itemReady = false,
};

// sorting stats singleton struct
// holds the tallies the different item classes
volatile struct {
	unsigned int totalCount;
	unsigned int itemClassCount[5];
} ItemStats = {
	.totalCount = 0,
	.itemClassCount = {0,0,0,0,0},
};

/* ====== MAIN FUNCTION ====== */

int main()
{
	// initial sequence
	Initialize();
	TIMER2_DelayMs(2000);
	DCMOTOR_Run(&belt,DCMOTOR_SPEED);
	
	// main loop
	while(true)
	{
		switch(fsmState.state)
		{
		case RUN_STATE:
			{	
				// In the Run State, the stepper motor target setting logic
				// and the the synchronization between the stepper motor and the rest of system
				
				// Only need to do anything if there are items to be processed
				if(!QUEUE_IsEmpty(readyQueue))
				{
					// belt lock means that that the tray is still rotating the previous item
					// this is because we drop the item when the tray is in range, not in position
					// if the belt is not locked we can acquire a new target
					if(!tray.beltLock)
					{
						// reset flag that indicates two idential items follow eachother
						tray.consecutiveItemsIdentical = false;
						
						// check what's in the queue, we will rotate to this if not already there
						ItemClass firstClass = QUEUE_Peak(readyQueue).class;
						
						// check the second item in the queue if it exists
						// we can alter our rotation pattern with this info
						ItemClass secondClass = UNCLASSIFIED;
						if (QUEUE_Size(readyQueue) > 1)
						{
							secondClass = QUEUE_PeakSecond(readyQueue).class;
							
							if (firstClass == secondClass)
								tray.consecutiveItemsIdentical = true;		
						}
						
						// acquire a new target if necessary, and begin the rotation by starting timer
						if (firstClass != TRAY_GetTarget(&tray))
						{
							TRAY_SetTarget(&tray, firstClass);
							TIMER2_DelayMs(tray.moveStartDelay);
							TIMER1_ScheduleIntUs(TRAY_INTERRUPT_INIT_DELAY); // interrupt later
							TIMER1_EnableInt();
						}
					}
					
					// if it is safe to drop an item (in range, item ready, and operation is valid for that item)
					if( !tray.beltLock && Stage3.itemReady && TRAY_inRange(&tray))
					{
						DCMOTOR_Run(&belt, DCMOTOR_SPEED);
						ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
						{
							//reset signal flag
							Stage3.itemReady = false;
							QueueElement dropItem = QUEUE_Dequeue(readyQueue);
							uint8_t index = (dropItem.class == UNCLASSIFIED) ? 4 : (dropItem.class / 50);
							(ItemStats.itemClassCount[index])++;
							
						}
						
						// if the tray isn't at target yet i.e. we dropped the item when the tray was in range
						if(!TRAY_IsReady(&tray))
						{
							// we need to lockout the belt so that that Exit ISR know to
							// brake if we aren't at target yet
							// but only if the next item isn't the same
							if(!tray.consecutiveItemsIdentical)
							{
								tray.beltLock = true;
							}
							
							// also set the appropriate timing delay
							tray.moveStartDelay = ITEM_READY_BEFORE_TRAY_DELAY;
						}
						else
						// if the tray is at its target, i.e. the tray was ready before
						// the item arrived at Exit Gate
						{
							// set the appropriate delay
							tray.moveStartDelay = TRAY_READY_BEFORE_ITEM_DELAY;
						}
					}
						
				}
				else
				{
					// if there was nothing in the queue, set the appropriate timing delay
					tray.moveStartDelay = FIRST_ITEM_IN_QUEUE_DELAY;
				}
				
				// once the tray is ready, we need to signal that in any case
				if(TRAY_IsReady(&tray)) tray.beltLock = false;
			}
			break;

		case PAUSE_STATE:
			{
				// We enter the Pause State from the INT7 interrupt
				LED_Set(0b000011000011);
				if (Stage2.badItemFlag)
				{
					Stage2.badItemFlag = false;
					UART_SendString("BAD ITEM ENCOUNTERED! PLEASE REMOVE IT!\r\n");
				}
				else
					UartDisplay(); // display to the terminal via uart
					
				while(fsmState.state == PAUSE_STATE); // wait while still in pause
				
				LED_Set(0x00);
			}
			break;
		
		case RAMPDOWN_STATE:
			{
				// we enter ramp-down once the timer 3 interrupt has triggered
				cli();
				DCMOTOR_Brake(&belt);
				UartDisplay(); // display stats on terminal via UART
				while(true)
				{
					// loop forever and display the output stats on LED
				}
			}
			break;
		}
	}
	
	// should never get here
	return 0;
}

// initialization sequence for all peripherals
void Initialize()
{
	cli(); // turn off interrupts
	// clear EXTI interrupt enables just in case of weird startup state
	EIMSK = 0x00;
	EICRA = 0x00;
	EICRB = 0x00;
	// ====== INIT CODE START ======
	// Initialize all required resources and peripherals
	SYSCLK_Init();
	LED_Init();
	TIMER1_Init();
	TIMER2_DelayInit();
	UART_Init();
	DCMOTOR_Init(&belt);
	ADC_Init(&adc, ADC_PRESCALE_128);
	FERRO_Init(&ferro);
	OPTICAL_Init(&s1_optic,S1_OPTICAL);
	OPTICAL_Init(&s2_optic,S2_OPTICAL);
	OPTICAL_Init(&exit_optic,EXIT_OPTICAL);
	BUTTON_Init(&pauseButton, PAUSE_BUTTON);
	BUTTON_Init(&rampDownButton, RAMPDOWN_BUTTON);
	TRAY_Init(&tray);
	TRAY_Home(&tray);
	
	FILTER_InitReset(1023.0K); // initialize to most likely first value;
	// in the future, could do an ADC run and set to the average value of the background found
	// perhaps in an ADC_Calibrate function
	
	// initialize both queues
	readyQueue = QUEUE_Create();
	processQueue = QUEUE_Create();
	
	// ====== INIT CODE END   ======
	Welcome();
	//wait for ramp up signal
	while(!BUTTON_IsPressed(&rampDownButton));
	TIMER2_DelayMs(500);
	BUTTON_EnableInt(); // enable button press interrupts
	sei(); // turn on interrupts
}


// classification algorithm for items
ItemClass Classify(QueueElement elem)
{	
	// choose the item based on whether it is metal or not,
	// and which range it falls into
	
	
	if( elem.isFerroMag )
	{
		if (elem.reflectivity < STEEL_ALUMINIUM_CUTOFF)
			return ALUMINIUM;
		else
			return STEEL;
		
	}
	else
	{	
		if (elem.reflectivity < BLACK_WHITE_CUTOFF)
		{
			// it can happen that a steel piece might not seem metal
			// if it was really close to another steel piece
			// if thats the case, correct that
			if (elem.reflectivity < METAL_CUTOFF_REFL)
				return STEEL;
			else
				return WHITE_PLASTIC;
		}
		else
			return BLACK_PLASTIC;
	}
}

// welcome screen displayed to serial terminal
void Welcome()
{
	UART_SendString("Hello! Welcome to Matt&Noah's nifty thingy that does things!\r\n\r\n");
	UART_SendString("The things this thingy does are pretty tight\r\n");
	UART_SendString("The makers of this thing hope very much that you enjoy how awesomely it does things,\r\n");
	UART_SendString("though we somewhat suspect it won't be that impressive...\r\n\r\n");
	UART_SendString("Matt and Noah would also like to thank Patrick Chang for all the help\r\n");
	UART_SendString("he has provided over the course of this project.\r\n\r\nEnjoy!\r\n\r\n");
	UART_SendString("=================================== SYSTEM READY =================================\r\n\r\n");
	UART_SendString("\tTO BEGIN: press the RIGHT BUTTON\r\n");
	UART_SendString("\tTO PAUSE and UNPAUSE: press the LEFT BUTTON\r\n");
	UART_SendString("\tTO END: press the RIGHT BUTTON again (system must be started)\r\n\r\n");
	UART_SendString("==================================================================================\r\n\r\n");
	
}

/// display for during pause and ramp-down to the terminal
void UartDisplay()
{
	char sendbuf[50];
	// display item stats over UART
	if (fsmState.state == PAUSE_STATE)
	{
		UART_SendString("============ SYSTEM PAUSED ============\r\n\r\n");
		UART_SendString("Total items (anywhere in system):\t");
		sprintf(sendbuf,"%d\r\n", ItemStats.totalCount);
		UART_SendString(sendbuf);
	}
	else if (fsmState.state == RAMPDOWN_STATE)
	{
		UART_SendString("============ SYSTEM RAMP DOWN =========\r\n\r\n");
	}
	
	UART_SendString("Sorted Items:\r\n");
	UART_SendString("\tBlack Plastic:\t");
	sprintf(sendbuf, "%d\r\n", ItemStats.itemClassCount[BLACK_PLASTIC/50]);
	UART_SendString(sendbuf);
	UART_SendString("\tWhite Plastic:\t");
	sprintf(sendbuf, "%d\r\n", ItemStats.itemClassCount[WHITE_PLASTIC/50]);
	UART_SendString(sendbuf);
	UART_SendString("\tSteel:\t");
	sprintf(sendbuf, "%d\r\n", ItemStats.itemClassCount[STEEL/50]);
	UART_SendString(sendbuf);
	UART_SendString("\tAluminium:\t");
	sprintf(sendbuf, "%d\r\n\r\n", ItemStats.itemClassCount[ALUMINIUM/50]);
	UART_SendString(sendbuf);
	
	UART_SendString("\tUnclassified:\t");
	sprintf(sendbuf, "%d\r\n\r\n", ItemStats.itemClassCount[4]);
	UART_SendString(sendbuf);
	
	if (fsmState.state == PAUSE_STATE)
	{
		UART_SendString("Classified items (not yet sorted):\t");
		sprintf(sendbuf, "%d\r\n\r\n", QUEUE_Size(readyQueue));
		UART_SendString(sendbuf);
		UART_SendString("Items in process (not yet classified):\t");
		sprintf(sendbuf, "%d\r\n\r\n", QUEUE_Size(processQueue));
		UART_SendString(sendbuf);
	}
	
	UART_SendString("========================================\r\n\r\n");
	
	if (fsmState.state == RAMPDOWN_STATE)
		UART_SendString("Goodbye.\r\n\r\n");
}

// allows triggerring pause from within code
void TriggerPauseManual()
{
	// effectively, everything between now, and the next time the button is pressed, is atomic
	fsmState.state = PAUSE_STATE;
	// save relevant system state, right now is just the belt status in case it was stopped
	fsmState.saved.beltWasRunning = belt.isRunning;
	DCMOTOR_Brake(&belt);
}

/* ====== INTERRUPT SERVICE ROUTINES ====== */

// ISR for S1_OPTICAL
ISR(INT1_vect)
{
	LED_Toggle(0);
	// verify interrupt wasn't spurious by polling sensor
	// this is critical as it helps to avoid enqueuing fictitious items
	if (OPTICAL_IsBlocked(&s1_optic))
	{	
		QueueElement new_elem = DEFAULT_QUEUE_ELEM;
		// increment total stat count and tag item with its count ID
		new_elem.counter = ++(ItemStats.totalCount);
		
		// enqueue is atomic
		QUEUE_Enqueue(processQueue, new_elem);
	}
	
	// reset the TIMER 3 counter register
	// this is for ramp-down sequence
	// so that if a new item arrives the count is reset
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		TCNT3 = 0x0000;
	}
}

// ISR for Ferro Sensor
ISR(INT5_vect)
{
	LED_Toggle(4);
	// verify interrupt wasn't spurious by polling sensor
	if (FERRO_Read(&ferro))
	{
		// don't need to check if queue is populated because it must be if we are here
		// call is atomic
		// get pointer to back of queue and assign metal identity
		QUEUE_BackPtr(processQueue)->isFerroMag = true;
	}
}

// ISR for S2_OPTICAL
ISR(INT2_vect)
{
	// poll sensor state to make sure not spurious
	
	if (OPTICAL_IsBlocked(&s2_optic))
	{
		Stage2.sampleCount = 0; // reset sample counter
		Stage2.minReflectivity = LARGEST_UINT16_T; // reset to default reflectivity
		FILTER_InitReset(1023.0K);
		ADC_StartConversion(&adc);
	}
}

// ISR for EXIT_OPTICAL
ISR(INT0_vect)
{
	LED_Toggle(7);
	// verify not spurious
    if(OPTICAL_IsBlocked(&exit_optic))
	{
		Stage3.itemReady = true;
		
		// brake the belt if the tray is still rotating for the previous object
		// or if the tray is not in range
		if(tray.beltLock) DCMOTOR_Brake(&belt);
		else if(!TRAY_inRange(&tray)) DCMOTOR_Brake(&belt);
	}
}

// ISR for RAMP_DOWN button
ISR(INT6_vect)
{
	if(!fsmState.rampDownInitFlag)
	{
		TIMER3_InterruptInit();
		fsmState.rampDownInitFlag = true;
	}
}

// ISR for PAUSE button
ISR(INT7_vect)
{
	if (fsmState.state != RAMPDOWN_STATE) // can't pause if in rampdown
	{	
		if (fsmState.state != PAUSE_STATE) // if we aren't already in pause
		{
			cli();
			fsmState.state = PAUSE_STATE;
			// save relevant system state
			// right now is just the belt status in case it was stopped
			fsmState.saved.beltWasRunning = belt.isRunning;
			DCMOTOR_Brake(&belt);
		}
		else
		{
			fsmState.state = RUN_STATE;
			
			// restore belt state
			if(fsmState.saved.beltWasRunning)
				DCMOTOR_Run(&belt, DCMOTOR_SPEED);
			
			LED_Set(0x00);
			sei(); // reenable interrupts
		}
	}
}

ISR(ADC_vect)
{
	// store result of adc conversion
	ADC_ReadConversion(&adc);
	Stage2.sampleCount++;
	
	// filter the result of the ADC conversion
	accum fx_out = Filter((accum)(adc.result));
	uint16_t u_out;
	
	// bound the result beween 0 and 1023 and convert to integer
	// (computations done in fixed point)
	if ( fx_out < 0.0K ) u_out = 0;
	else if (fx_out > 1023.0K) u_out = 1023;
	else u_out = (uint16_t)fx_out;
	
	// track minimum reflectivity
	if (u_out < Stage2.minReflectivity)
	Stage2.minReflectivity = u_out;
	
	// if the item is still in front of the sensor, start another conversion
	if (OPTICAL_IsBlocked(&s2_optic))
	{
		ADC_StartConversion(&adc);
	}
	else // last ADC sample, process and dequeue
	{
		//move item from the "process Queue", classify, and move to the "ready" Queue
		QueueElement processedItem = QUEUE_Dequeue(processQueue);
		// store minimum reflectivity and sample count in item
		processedItem.reflectivity = Stage2.minReflectivity;
		processedItem.sampleCount = Stage2.sampleCount;
		
		processedItem.class = Classify(processedItem);
		
		// verify not a bad item
		if (Stage2.badItemFlag == true)
		{
			TriggerPauseManual();
			ItemStats.totalCount--;
		}
		else
		{
			// Atomic enqueue
			QUEUE_Enqueue(readyQueue, processedItem);
		}
	}
}


// ISR for stepper motor commutation
ISR(TIMER1_COMPA_vect)
{
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		TIMER1_DisableInt();
		
		// simply steps the motor and figures out the next delay
		// reenables interrupts if necessary
		TRAY_Process(&tray);
	}
}

// ISR for entering ramp down once all processing is finished
ISR(TIMER3_COMPA_vect)
{
	fsmState.state = RAMPDOWN_STATE;
}


ISR(BADISR_vect)
{
	while(1)
	{
		LED_Set( 0b01010101);
		TIMER2_DelayMs(500);
		LED_Set( 0b10101010);
		TIMER2_DelayMs(500);
	}
}


