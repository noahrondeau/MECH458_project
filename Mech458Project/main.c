

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

/* ====== MAIN-LOCAL DEFINITIONS ====== */

/* ====== FUNCTION PROTOTYPES ====== */
void Initialize();
ItemClass Classify(QueueElement elem);

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
FsmState fsmState = {
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


/* ====== MAIN FUNCTION ====== */

int main()
{
	Initialize();
	TRAY_Home(&tray);
	TIMER1_DelayMs(2000);
	DCMOTOR_Run(&belt,DCMOTOR_SPEED);
	
	// main loop
	while(true)
	{	
		switch(fsmState.state)
		{
		case RUN_STATE:
			{
				if ( !QUEUE_IsEmpty(readyQueue))
				{
					// the tray is not in position! rotate!
					ItemClass nextClass = QUEUE_Peak(readyQueue).class;
				
					// update target and turn belt
					if ( nextClass != UNCLASSIFIED && nextClass != TRAY_GetTarget(&tray))
					{
						TRAY_Sort(&tray, nextClass); // this waits a lot and updates the target
					}
				
					//do a safe read
					bool itemReady = Stage3.itemReady;
					while( itemReady != Stage3.itemReady)
						itemReady = Stage3.itemReady;
				
					// if the item is ready, dequeue the item	
					if(itemReady)
					{
						QueueElement dropItem = QUEUE_Dequeue(readyQueue);
						if (dropItem.class == UNCLASSIFIED)
							LED_Set(0xFF);
						// sort stats here later!
					
						Stage3.itemReady = false;
						// if in this time the EXIT interrupt fired, the belt would have been turned off,
						// so turn it on again (if already on, this has no effect)
						DCMOTOR_Run(&belt, DCMOTOR_SPEED);
					}
				}
			}
			break;
			
		case PAUSE_STATE:
			{
				// nothing lets us get here right now
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
	// for now, if the item falls within one of the ranges, we classify it
	// as the material corresponding to that range.
	// this will have to be improved upon, since overlap can occur
	
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
		return UNCLASSIFIED; // this should never be reached
}

/* ====== INTERRUPT SERVICE ROUTINES ====== */

// ISR for S1_OPTICAL
ISR(INT1_vect)
{
	// verify interrupt wasn't spurious by polling sensor
	// this is critical as it helps to avoid enqueuing fictitious items
	if (OPTICAL_IsBlocked(&s1_optic))
	{	
		//LED_Toggle( 0);
		
		QueueElement new_elem = DEFAULT_QUEUE_ELEM;
		// increment total stat count and tag item with its count ID
		new_elem.counter = ++(ItemStats.totalCount);
		// initialize like this so that if no ferro interrupt flips then we are good
		new_elem.isFerroMag = false;
		QUEUE_Enqueue(processQueue, new_elem);
	}
}

// ISR for Ferro Sensor
ISR(INT3_vect)
{
	// verify interrupt wasn't spurious by polling sensor
	if (FERRO_Read(&ferro))
	{
		// don't need to check if queue is populated because it must be if we are here
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
		QueueElement processedItem = QUEUE_Dequeue(processQueue);
		// store minimum reflectivity and sample count in item
		processedItem.reflectivity = Stage2.minReflectivity;
		processedItem.sampleCount = Stage2.sampleCount;
		
			
		//classify item and move to ready queue
		processedItem.class = Classify(processedItem);
		QUEUE_Enqueue(readyQueue, processedItem);
	}
}

// ISR for EXIT_OPTICAL
ISR(INT0_vect)
{
	// verify not spurious
    if(OPTICAL_IsBlocked(&exit_optic))
	{
		// signal that an item is ready
		Stage3.itemReady = true;
		
		// check tray readiness in a thread-safe way by reading twice
		// this is allowed because tray-ready is 1 owner, mutiple client, only modified by the Tray object
		bool trayReady = TRAY_IsReady(&tray);
		bool trayReadyNew;
		while( trayReady != ( trayReadyNew = TRAY_IsReady(&tray))) // the ASSIGNMENT IS ON PURPOSE!!!!
		{ 
			trayReady = trayReadyNew;
		}
		
		// stop the belt if not ready
		if (!trayReady)
		{
			DCMOTOR_Brake(&belt);
		}
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
	LED_Toggle( 7);
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


