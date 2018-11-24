

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

/* ====== MAIN-LOCAL DEFINITIONS ====== */

/* ====== FUNCTION PROTOTYPES ====== */
void Initialize();

/* ====== GLOBAL SYSTEM RESOURCES ====== */

// all these types are typedef'd as volatile, don't need to repeat here
LedBank			led;
DcMotor			belt;
ADCHandle		adc;
FerroSensor		ferro;
OpticalSensor	s1_optic;
OpticalSensor	s2_optic;
OpticalSensor	exit_optic;
PushButton		pauseButton;
PushButton		rampDownButton;
Tray			tray;

FsmState fsmState = {
	.state = RUN_STATE,
	.rampDownInitFlag = false,
	.rampDownEndFlag = false,
};

Queue* readyQueue;
Queue* processQueue;

volatile struct {
	uint16_t minReflectivity;
} Stage2 = {
	.minReflectivity = MAX_ADC_VAL,
};

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
	//DCMOTOR_Run(&belt,DCMOTOR_SPEED);
		

	// main loop
	while(true)
	{	
		switch(fsmState.state)
		{
		case RUN_STATE:
			{
				//0-15
				for(int i=0; i<14; i++)
				{
					STEPPER_StepCW(&(tray.stepper));
					TIMER1_DelayMs(21-1*i);
				}
				//16-35
				for(int i=0;  i<25; i++)
				{
					STEPPER_StepCW(&(tray.stepper));
					TIMER1_DelayMs(8);
				}
				//35-50
				for(int i=0;  i<14; i++)
				{
					STEPPER_StepCW(&(tray.stepper));
					TIMER1_DelayMs(8+1*i);
				}
				TIMER1_DelayMs(1000);
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
	SYSCLK_Init();
	LED_Init(&led);
	TIMER1_DelayInit();
	TIMER3_DelayInit();
	DCMOTOR_Init(&belt);
	ADC_Init();
	FERRO_Init(&ferro);
	OPTICAL_Init(&s1_optic,S1_OPTICAL);
	OPTICAL_Init(&s2_optic,S2_OPTICAL);
	OPTICAL_Init(&exit_optic,EXIT_OPTICAL);
	BUTTON_Init(&pauseButton, PAUSE_BUTTON);
	BUTTON_Init(&rampDownButton, RAMPDOWN_BUTTON);
	TRAY_Init(&tray);
	
	
	readyQueue = QUEUE_Create();
	processQueue = QUEUE_Create();
	
	DDRD |= 0xF0;
	PORTD = (PORTD & 0x0F);
	
	// ====== INIT CODE END   ======
	sei(); // turn on interrupts
}


/* ====== INTERRUPT SERVICE ROUTINES ====== */

// ISR for S1_OPTICAL
ISR(INT1_vect)
{
	// verify interrupt wasn't spurious by polling sensor
	// this is critical as it helps to avoid enqueuing fictitious items
	if (OPTICAL_IsBlocked(&s1_optic))
	{	
		//LED_toggle(&led, 0);
		
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
		//LED_toggle(&led, 1);
		QUEUE_BackPtr(processQueue)->isFerroMag = true;
	}
}

// ISR for S2_OPTICAL
ISR(INT2_vect)
{
	// poll sensor state to determine whether falling or rising edge was seen
	
	if (OPTICAL_IsBlocked(&s2_optic)) //just saw falling edge
	{
		//LED_toggle(&led, 2);
		ADC_StartConversion(&adc);
	}
	else // just saw rising edge
	{
		//LED_toggle(&led,3);
		
		//move item from the "process Queue" to the "ready Queue"
		if (!QUEUE_IsEmpty(processQueue))
		{
			QueueElement processedItem = QUEUE_Dequeue(processQueue);
			//add reflectivity for now -- calculate class here!
			processedItem.reflectivity = Stage2.minReflectivity;
			Stage2.minReflectivity = MAX_ADC_VAL;
			QUEUE_Enqueue(readyQueue, processedItem);
		}
	}
}

// ISR for EXIT_OPTICAL
ISR(INT0_vect)
{
	//LED_toggle(&led, 4);
	// poll sensor to verify interrupt was not spurious
/*	if(OPTICAL_IsBlocked(&exit_optic))
	{
		DCMOTOR_Brake(&belt);
		// even if the interrupt is not spurious and we know there is an item
		// verify there is a queued item
		if(!QUEUE_IsEmpty(readyQueue))
		{
			QueueElement dropItem = QUEUE_Dequeue(readyQueue);
			LED_set(&led, (uint8_t)((dropItem.reflectivity) >> 2));
			
			//Testing sort using hard set values
			dropItem.class = STEEL;	
			TRAY_Sort(&tray,dropItem);
						
			//uint8_t MSB1 = (uint8_t)((dropItem.reflectivity >> 9) << 7);
			//uint8_t MSB0 = (uint8_t)(((dropItem.reflectivity >> 8) & 0b00000001) << 5);
			//PORTD = (PORTD & 0x0F) | MSB1 | MSB0;
			
			TIMER1_DelayMs(1000);
			LED_set(&led, 0x00);
			DCMOTOR_Run(&belt, DCMOTOR_SPEED);
		}
	}*/


	//test 1:
	//readyQueue->front->class = tray.beltPos;
	
	//test 2:
	//readyQueue->front->class = STEEL;
	
	//test 3:
	//readyQueue->front->class = ALUMINIUM;
	
	//test 4:
	//readyQueue->front->class = BLACK_PLASTIC;
	
	//test 5:
	//readyQueue->front->class = WHITE_PLASTIC;
	
	//Random test
	switch (rand()%4)
	{
		case 0:
			readyQueue->front->class = WHITE_PLASTIC;
			LED_toggle(&led,0);
			break;
		case 1:
			readyQueue->front->class = STEEL;
			LED_toggle(&led,1);
			break;
		case 2:
			readyQueue->front->class = ALUMINIUM;
			LED_toggle(&led,2);
			break;
		case 3:
			readyQueue->front->class = BLACK_PLASTIC;
			LED_toggle(&led,3);
			break;						
	}
	
	//LED Toggle for current Belt Position
	switch (tray.beltPos)
	{
		case WHITE_PLASTIC:
			LED_toggle(&led,4);
			break;
		case STEEL:
			LED_toggle(&led,5);
			break;
		case ALUMINIUM:
			LED_toggle(&led,6);
			break;
		case BLACK_PLASTIC:
			LED_toggle(&led,7);
		break;
	}

	if(OPTICAL_IsBlocked(&exit_optic))
	{
		if(tray.beltPos == QUEUE_Peak(readyQueue).class)
		{
			if(!QUEUE_IsEmpty(readyQueue))
			{
				QueueElement dropItem = QUEUE_Dequeue(readyQueue);
				TRAY_Sort(&tray, &dropItem);
			}
		}
		else if(tray.beltPos != QUEUE_Peak(readyQueue).class)
		{
			
			DCMOTOR_Brake(&belt);
			
			if(!QUEUE_IsEmpty(readyQueue))
			{
				QueueElement dropItem = QUEUE_Dequeue(readyQueue);
				TRAY_Sort(&tray, &dropItem);
				DCMOTOR_Run(&belt, DCMOTOR_SPEED);
			}			
		}
	}
	
	LED_set(&led,0x00);

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
	LED_toggle(&led, 6);
	TIMER3_DelayMs(20);
}

// ISR for PAUSE button
ISR(INT7_vect)
{
	// Debounce
	// We should probably set up a new different timer for this
	// Since this one will be used for the stepper motor
	TIMER3_DelayMs(20);
	LED_toggle(&led, 7);
	TIMER3_DelayMs(20);
}

ISR(ADC_vect)
{
	ADC_ReadConversion(&adc);
	//LED_set(&led, (uint8_t)((adc.result) >> 2));
	if (adc.result < Stage2.minReflectivity)
		Stage2.minReflectivity = adc.result;
	
	if (OPTICAL_IsBlocked(&s2_optic))
		ADC_StartConversion(&adc);
}

/*
ISR(TIMER3_COMPB_vect)
{
	
	LED_toggle(&led,1);
	TCNT3 = 0x0000;			//reset counter
	TIFR3 |= _BV(OCF3B);	//Clear interrupt flag begin counting
	
	PORTC = 0xFF;
}
*/

ISR(BADISR_vect)
{
	while(1)
	{
		LED_set(&led, 0b01010101);
		TIMER1_DelayMs(500);
		LED_set(&led, 0b10101010);
		TIMER1_DelayMs(500);
	}
}


