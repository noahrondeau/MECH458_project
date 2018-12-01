

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
DigitalFilter	adcFilter; // butterworth LPF for ADC vals

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
	ItemClass currDispType;
} DisplayStatus = {
	.currDispType = 0,
};


/* ====== MAIN FUNCTION ====== */

int main()
{
	Initialize();
	TIMER1_DelayMs(2000);
	//DCMOTOR_Run(&belt,DCMOTOR_SPEED);
	
	uint8_t testIndex = 0;
	uint8_t testPos[7] = {100,0,150,100,50,150,0};
	// main loop
	while(true)
	{	
		switch(fsmState.state)
		{
		case RUN_STATE:
			{
				TRAY_Sort(&tray);
				
				if(TRAY_IsReady(&tray))
				{
					//TIMER1_DelayMs(500);
					LED_Set(tray.lastDelay);
					TRAY_SetTarget(&tray, testPos[testIndex]);
					testIndex = (testIndex + 1) %7;
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
				// nothing lets us get here right now
				return 0;
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
	TIMER1_DelayInit();
	TIMER2_DelayInit();
	TIMER3_DelayInit();
	DCMOTOR_Init(&belt);
	ADC_Init(&adc, ADC_PRESCALE_32);
	FERRO_Init(&ferro);
	OPTICAL_Init(&s1_optic,S1_OPTICAL);
	OPTICAL_Init(&s2_optic,S2_OPTICAL);
	OPTICAL_Init(&exit_optic,EXIT_OPTICAL);
	BUTTON_Init(&pauseButton, PAUSE_BUTTON);
	BUTTON_Init(&rampDownButton, RAMPDOWN_BUTTON);
	TRAY_Init(&tray);
	TRAY_Home(&tray);
	
	// initialize filter
	float num[] = FILTER_NUMER_COEFFS;
	float den[] = FILTER_DENOM_COEFFS;
	FILTER_Init(&adcFilter, num, den, 1023); // initialize to most likely first value;
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
	static ItemClass test = 50;
	// for now, if the item falls within one of the ranges, we classify it
	// as the material corresponding to that range.
	// this will have to be improved upon, since overlap can occur
	/*
	if		(	elem.reflectivity >= MIN_ALUMINIUM_VAL
				&& elem.reflectivity <= MAX_ALUMINIUM_VAL )
		return ALUMINIUM;
		 
	else if (	elem.reflectivity >= MIN_STEEL_VAL
				&& elem.reflectivity <= MAX_STEEL_VAL )
		return  STEEL;
	
	else if (	elem.reflectivity >= MIN_WHITE_PLASTIC_VAL
				&& elem.reflectivity <= MAX_WHITE_PLASTIC_VAL )
		return  WHITE_PLASTIC;
	
	else if (	elem.reflectivity >= MIN_BLACK_PLASTIC_VAL
				&& elem.reflectivity <= MAX_BLACK_PLASTIC_VAL )
		return  BLACK_PLASTIC;
	else
		return UNCLASSIFIED; // this should never be reached*/
	
	test = (test + 50) % 200;
	return test;
}

void PauseDisplay()
{
	// right now, just LED, in the future could be UART
	switch(DisplayStatus.currDispType)
	{
	case ALUMINIUM:
		LED_SetBottom8(0b10000000 | (ItemStats.aluminiumCount & 0x0F));
		break;
	case STEEL:
		LED_SetBottom8(0b01000000 | (ItemStats.steelCount & 0x0F));
		break;
	case WHITE_PLASTIC:
		LED_SetBottom8(0b00100000 | (ItemStats.whitePlasticCount & 0x0F));
		break;
	case BLACK_PLASTIC:
		LED_SetBottom8(0b00010000 | (ItemStats.blackPlasticCount & 0x0F));
		break;
	default:
		break;
	}
	// on next loop through, display this
	DisplayStatus.currDispType = (DisplayStatus.currDispType + 50) % 200; 
	TIMER1_DelayMs(1000);
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
	// poll sensor state to determine whether falling or rising edge was seen
	
	if (OPTICAL_IsBlocked(&s2_optic)) //just saw falling edge
	{
		Stage2.adcContinueConversions = true;
		Stage2.sampleCount = 0; // reset sample counter
		Stage2.minReflectivity = LARGEST_UINT16_T; // reset to default reflectivity
		FILTER_ResetWithPadding(&adcFilter, 1023);
		ADC_StartConversion(&adc);
	}
	else // just saw rising edge
	{
		// stop the ADC from converting
		Stage2.adcContinueConversions = false;
		
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
		LED_Set(QUEUE_Size(readyQueue));
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
		if (TRAY_IsReady(&tray))
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
	TIMER3_DelayMs(20);
	LED_Toggle(6);
	TIMER3_DelayMs(20);
}

// ISR for PAUSE button
ISR(INT7_vect)
{
	// Debounce
	// We should probably set up a new different timer for this
	// Since this one will be used for the stepper motor
	TIMER3_DelayMs(20);
	
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
	TIMER3_DelayMs(20);
}

ISR(ADC_vect)
{
	if (Stage2.adcContinueConversions)
	{
		ADC_ReadConversion(&adc);
		//LED_Set( (uint8_t)((adc.result) >> 2));
		Stage2.sampleCount++;
	
		float f_filteredOutput = Filter(&adcFilter, adc.result);
		uint16_t u_filteredOutput;
	
		if ( f_filteredOutput < 0.0 ) u_filteredOutput = 0;
		else if (f_filteredOutput > 1023.0) u_filteredOutput = 1023;
		else u_filteredOutput = (uint16_t)f_filteredOutput;
	
		if (u_filteredOutput < Stage2.minReflectivity)
			Stage2.minReflectivity = u_filteredOutput;
		
		if (OPTICAL_IsBlocked(&s2_optic))
			ADC_StartConversion(&adc);
	}
}

/*
ISR(TIMER3_COMPB_vect)
{
	
	LED_Toggle(1);
	TCNT3 = 0x0000;			//reset counter
	TIFR3 |= _BV(OCF3B);	//Clear interrupt flag begin counting
	
	PORTC = 0xFF;
}
*/

ISR(BADISR_vect)
{
	while(1)
	{
		LED_Set( 0b01010101);
		TIMER1_DelayMs(500);
		LED_Set( 0b10101010);
		TIMER1_DelayMs(500);
	}
}


