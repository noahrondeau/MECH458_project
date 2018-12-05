

/* ====== LIBRARY INCLUDES ====== */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>

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

/* ====== MAIN-LOCAL DEFINITIONS ====== */

/* ====== FUNCTION PROTOTYPES ====== */
void Initialize();
ItemClass Classify(QueueElement elem);
void PauseDisplay();
void RampDisplay();

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
} Stage2 = {
	.minReflectivity = LARGEST_UINT16_T,
	.sampleCount = 0,
	.adcContinueConversions = false,
};

volatile struct {
	bool itemReady;
} Stage3 = {
	.itemReady = false,
};

// sorting stats singleton struct
volatile struct {
	unsigned int totalCount;
	unsigned int blackPlasticCount;
	unsigned int whitePlasticCount;
	unsigned int aluminiumCount;
	unsigned int steelCount;
} ItemStats = {
	.totalCount = 0,
	.blackPlasticCount = 0,
	.whitePlasticCount = 0,
	.aluminiumCount = 0,
	.steelCount = 0,
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
				
				if(!QUEUE_IsEmpty(readyQueue))
				{
					ItemClass nextClass = QUEUE_Peak(readyQueue).class;
					if (nextClass != TRAY_GetTarget(&tray))
					{
						TRAY_SetTarget(&tray, nextClass);
						TIMER1_ScheduleIntUs(20000); // immediately fire an interrupt to get the tray started
						TIMER1_EnableInt();
					}
					
					if (Stage3.itemReady && TRAY_IsReady(&tray))
					{
						ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
						{
							Stage3.itemReady = false;
							QUEUE_Dequeue(readyQueue);
							DCMOTOR_Run(&belt, DCMOTOR_SPEED);
						}
					}
				}
			}
			break;

		case PAUSE_STATE:
			{
				PauseDisplay();
			}
			break;
		
		case RAMPDOWN_STATE:
			{
				cli();
				DCMOTOR_Brake(&belt);
				RampDisplay();
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
		accum z_alum  = (refl - AVG_ALUMINIUM_VAL) / STDEV_ALUMINIUM;
		accum z_steel = (refl - AVG_STEEL_VAL) / STDEV_STEEL;
		
		if( absk(z_alum) <= absk(z_steel) )
			return ALUMINIUM;
		else
			return STEEL;
	}
	else
	{
		accum z_white = (refl - AVG_WHITE_VAL) / STDEV_WHITE;
		accum z_black = (refl - AVG_BLACK_VAL) / STDEV_BLACK;
		
		if( absk(z_white) <= absk(z_black) )
			return WHITE_PLASTIC;
		else
			return BLACK_PLASTIC;
	}
}

void RampDisplay()
{
	// right now, just LED, in the future could be UART
	switch(DisplayStatus.currDispType)
	{
	case BLACK_PLASTIC:
		LED_SetBottom8(0b00010000 | (ItemStats.blackPlasticCount & 0x0F));
		break;		
	case ALUMINIUM:
		LED_SetBottom8(0b00100000 | (ItemStats.aluminiumCount & 0x0F));
		break;
	case WHITE_PLASTIC:
		LED_SetBottom8(0b01000000 | (ItemStats.whitePlasticCount & 0x0F));
		break;
	case STEEL:
		LED_SetBottom8(0b10000000 | (ItemStats.steelCount & 0x0F));
		break;
	default:
		break;
	}
	// on next loop through, display this
	DisplayStatus.currDispType = (DisplayStatus.currDispType + 50) % 200; 
	TIMER2_DelayMs(1000);
}

void PauseDisplay()
{
	// right now, just LED, in the future could be UART
	switch(DisplayStatus.currDispType)
	{
		case BLACK_PLASTIC:
		LED_SetBottom8(0b00010000 | (ItemStats.blackPlasticCount & 0x0F));
		break;
		case ALUMINIUM:
		LED_SetBottom8(0b00100000 | (ItemStats.aluminiumCount & 0x0F));
		break;
		case WHITE_PLASTIC:
		LED_SetBottom8(0b01000000 | (ItemStats.whitePlasticCount & 0x0F));
		break;
		case STEEL:
		LED_SetBottom8(0b10000000 | (ItemStats.steelCount & 0x0F));
		break;
		case 200:
		LED_SetBottom8(0b10010000 | (((uint8_t)QUEUE_Size(readyQueue) + (uint8_t)QUEUE_Size(processQueue)) & 0x0F));
		default:
		break;
	}
	// on next loop through, display this
	DisplayStatus.currDispType = (DisplayStatus.currDispType + 50) % 250;
	TIMER2_DelayMs(1000);
}
/* ====== INTERRUPT SERVICE ROUTINES ====== */

// ISR for S1_OPTICAL
ISR(INT1_vect)
{
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

	// verify not spurious
    if(OPTICAL_IsBlocked(&exit_optic))
	{
		// signal that an item is ready
		// call doesn't need to be atomic, we are in the highest priority interrupt
		Stage3.itemReady = true;
		
		// this is an atomic call
		// check if the tray is in position, if not stop the belt
		if (!TRAY_IsReady(&tray))
			DCMOTOR_Brake(&belt);
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
	// Debounce
	// We should probably set up a new different timer for this
	// Since this one will be used for the stepper motor
	TIMER2_DelayMs(20);
	if(!fsmState.rampDownInitFlag)
	{
		TIMER3_InterruptInit();
		fsmState.rampDownInitFlag = true;
	}
	TIMER2_DelayMs(20);
}

// ISR for PAUSE button
ISR(INT7_vect)
{
	// Debounce
	// We should probably set up a new different timer for this
	// Since this one will be used for the stepper motor
	TIMER2_DelayMs(20);
	
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
			sei(); // reenable interrupts
		}
	}
	TIMER2_DelayMs(20);
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
		// Atomic enqueue
		QUEUE_Enqueue(readyQueue, processedItem);	
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


