

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

/* ====== MAIN-LOCAL DEFINITIONS ====== */

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

volatile struct {
	bool itemReady;
} Stage3 = {
	.itemReady = false,
};

// sorting stats singleton struct
volatile struct {
	unsigned int totalCount;
	unsigned int itemClassCount[5];
} ItemStats = {
	.totalCount = 0,
	.itemClassCount = {0,0,0,0,0},
};

// for keeping track of whats going on in the display
volatile struct 
{
	uint8_t currDispType;
} DisplayStatus = {
	.currDispType = 0,
};

/* ====== MAIN FUNCTION ====== */

int main()
{
	
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
				//Tray Rotate if queue is not empty
				if(!QUEUE_IsEmpty(readyQueue))
				{
					if(!tray.beltLock)
					{
						// we can reset this here because it only matters if the belt is locked:
						// we only need this on to signal not to beltLock, once that is accomplished it won't matter
						tray.consecutiveItemsIdentical = false;
						// check what's in the queue, we will rotate to this if not already there
						ItemClass firstClass = QUEUE_Peak(readyQueue).class;
						
						// check the second item in the queue if it exists
						// we can alter our patter with this info
						ItemClass secondClass = UNCLASSIFIED;
						if (QUEUE_Size(readyQueue) > 1)
						{
							secondClass = QUEUE_PeakSecond(readyQueue).class;
							
							if (firstClass == secondClass)
								tray.consecutiveItemsIdentical = true;		
						}
						
						// =====temporary=====
						/*
						char sendString[50];
						sprintf(sendString, "#1: %u,\t#2: %u\r\n", (uint8_t)firstClass, (uint8_t)secondClass);
						UART_SendString(sendString);
						// =====*/
						
						if (firstClass != TRAY_GetTarget(&tray))
						{
							TRAY_SetTarget(&tray, firstClass);
							TIMER2_DelayMs(tray.moveStartDelay);
							TIMER1_ScheduleIntUs(TRAY_INTERRUPT_INIT_DELAY); // immediately fire an interrupt to get the tray started
							TIMER1_EnableInt();
						}
					}
					
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
							
							/* // for debugging purposes
							char sendbuf[20];
							sprintf(sendbuf, "%u\r\n", dropItem.sampleCount);
							UART_SendString(sendbuf);
							*/
						}
						// if the tray isn't at target yet i.e. we dropped the item when the tray was in range
						if(!TRAY_IsReady(&tray))
						{
							// we need to lockout the belt so that that Exit ISR know to brake if we aren't at target yet
							// but only if the next item isn't the same
							if(!tray.consecutiveItemsIdentical)
							{
								tray.beltLock = true;
							}
							
							// also set the appropriate timing delay
							tray.moveStartDelay = ITEM_READY_BEFORE_TRAY_DELAY;
						}
						else // if the tray is at its target, i.e. the tray was ready before the item arrived at Exit Gate
						{
							// set the appropriate delay
							tray.moveStartDelay = TRAY_READY_BEFORE_ITEM_DELAY;
						}
					}
						
				}
				else
				{
					tray.moveStartDelay = FIRST_ITEM_IN_QUEUE_DELAY;
				}
				
				if(TRAY_IsReady(&tray)) tray.beltLock = false;
			}
			break;

		case PAUSE_STATE:
			{
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
				cli();
				DCMOTOR_Brake(&belt);
				UartDisplay(); // display stats on terminal via UART
				while(true)
				{// loop forever and display the output stats on LED
				}
			}
			break;
		}
	}
	
	// should never get here
	return 0;
}

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

ItemClass Classify(QueueElement elem)
{	
	// calculate the z_scores of the object in each class' normal distribution
	// the smaller z_score indicates higher likelihood of the object type
	// this method works best if we have lots of data
	// use fixed-point arithmetic types supported by avr-gcc for this
	// NOTE: accum is unsigned
	accum refl = (accum)(elem.reflectivity);
	
	if( elem.isFerroMag )
	{
		// if a bad item is encountered (e.g. metal but not reflective enough)
		// then signal the condition
		/*
		if (refl > METAL_CUTOFF_REFL)
		{
			Stage2.badItemFlag = true;
			return UNCLASSIFIED;
		}*/
		/*
		accum z_alum  = (refl - AVG_ALUMINIUM_VAL) / STDEV_ALUMINIUM;
		accum z_steel = (refl - AVG_STEEL_VAL) / STDEV_STEEL;
		
		if( absk(z_alum) <= absk(z_steel) )
			return ALUMINIUM;
		else
			return STEEL;
			*/
		if (elem.reflectivity < STEEL_ALUMINIUM_CUTOFF)
			return ALUMINIUM;
		else
			return STEEL;
	}
	else
	{
		// if not ferromagnetic check that this makes sense
		/*
		if (refl < METAL_CUTOFF_REFL)
		{
			Stage2.badItemFlag = true;
			return UNCLASSIFIED;
		}*/
		
		accum z_white = (refl - AVG_WHITE_VAL) / STDEV_WHITE;
		accum z_black = (refl - AVG_BLACK_VAL) / STDEV_BLACK;
		
		if( absk(z_white) <= absk(z_black) )
			return WHITE_PLASTIC;
		else
			return BLACK_PLASTIC;
	}
}

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
		if(tray.beltLock) DCMOTOR_Brake(&belt);
		else if(!TRAY_inRange(&tray)) DCMOTOR_Brake(&belt);
	}
}

/*
// ISR for HALL Sensor UNUSED
ISR(INT4_vect)
{
	
}*/

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
	// this may need to be altered if we start doing some of the heavier computation in main
	// what happens if here we change the state variable when a classification is pending?
	// save the state variable? how do we distinguish between a state currently in execution, and a state that is pending?
	// maybe always keep track of what the next state should be?
	//
	// also note that as it stands, the motor control in main will complete its move to target
	// before we enter the pause state loop
	// this will get optimized out
	if (fsmState.state != RAMPDOWN_STATE)
	{	
		if (fsmState.state != PAUSE_STATE)
		{
			cli(); // immediately turn off interrupts so that nothing can change the state variable
			// effectively, everything between now, and the next time the button is pressed, is atomic
			fsmState.state = PAUSE_STATE;
			// save relevant system state, right now is just the belt status in case it was stopped
			fsmState.saved.beltWasRunning = belt.isRunning;
			DCMOTOR_Brake(&belt);
		}
		else
		{
			fsmState.state = RUN_STATE; // see comment above, may need to make it some saved value
			if(fsmState.saved.beltWasRunning)
				DCMOTOR_Run(&belt, DCMOTOR_SPEED);
				LED_Set(0x00);
			sei(); // reenable interrupts
		}
	}
}

ISR(ADC_vect)
{
	ADC_ReadConversion(&adc);
	Stage2.sampleCount++;
	
	accum fx_out = Filter((accum)(adc.result));
	uint16_t u_out;
	
	if ( fx_out < 0.0K ) u_out = 0;
	else if (fx_out > 1023.0K) u_out = 1023;
	else u_out = (uint16_t)fx_out;
	
	if (u_out < Stage2.minReflectivity)
	Stage2.minReflectivity = u_out;
	
	if (OPTICAL_IsBlocked(&s2_optic))
	{
		ADC_StartConversion(&adc);
	}
	else // last ADC sample, process and dequeue
	{
		//move item from the "process Queue", classify, and move to the "ready" Queue
		// dequeue -- no need to check if we can, because we must if we got the interrupt
		// call is atomic
		QueueElement processedItem = QUEUE_Dequeue(processQueue);
		// store minimum reflectivity and sample count in item
		processedItem.reflectivity = Stage2.minReflectivity;
		processedItem.sampleCount = Stage2.sampleCount;
		//classify item and move to ready queue
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
		// reenables interrupts if necessary
		TRAY_Process(&tray);
	}
}


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


