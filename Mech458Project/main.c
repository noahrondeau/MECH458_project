

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

/* ====== MAIN-LOCAL DEFINITIONS ====== */

/* ====== FUNCTION PROTOTYPES ====== */
void Initialize();

/* ====== GLOBAL SYSTEM RESOURCES ====== */

LedBank			led;
DcMotor			belt;
ADCHandle		adc;
FerroSensor		ferro;
HallSensor		hall;
OpticalSensor	s1_optic;
OpticalSensor	s2_optic;
OpticalSensor	exit_optic;
PushButton		pauseButton;
PushButton		rampDownButton;

volatile FsmState fsmState = MOTOR_CONTROL;
Queue* readyQueue;
Queue* processQueue;

volatile struct {
	uint16_t minReflectivity;
} Stage2 = {.minReflectivity = 1024,};

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
	TIMER1_DelayMs(2000);
	DCMOTOR_Run(&belt,100);

	
	while(1)
	{

	}
	return 0;
}

void Initialize()
{
	cli(); // turn off interrupts
	// clear interrupt enables just in case of weird startup state
	EIMSK = 0x00;
	EICRA = 0x00;
	EICRB = 0x00;
	// ====== INIT CODE START ======
	Sys_Clk_Init();
	LED_Init(&led);
	TIMER1_DelayInit();
	TIMER3_DelayInit();
	DCMOTOR_Init(&belt);
	//ADC_Init();
	FERRO_Init(&ferro);
	HALL_Init(&hall);
	OPTICAL_Init(&s1_optic,S1_OPTICAL);
	OPTICAL_Init(&s2_optic,S2_OPTICAL);
	OPTICAL_Init(&exit_optic,EXIT_OPTICAL);
	BUTTON_Init(&pauseButton, PAUSE_BUTTON);
	BUTTON_Init(&rampDownButton, RAMPDOWN_BUTTON);
	
	readyQueue = QUEUE_create();
	processQueue = QUEUE_create();
	
	DDRD |= 0xF0;
	PORTD = (PORTD & 0x0F);
	
	// ====== INIT CODE END   ======
	sei(); // turn on interrupts
}


/* ====== INTERRUPT SERVICE ROUTINES ====== */

// ISR for S1_OPTICAL
ISR(INT1_vect)
{
	//if (OPTICAL_IsBlocked(&s1_optic))
	//{	
		//LED_toggle(&led, 0);
		/*
		QueueElement new_elem = DEFAULT_QUEUE_ELEM;
		// increment total stat count and tag item with its count ID
		new_elem.counter = ++(ItemStats.totalCount);
		// initialize like this so that if no ferro interrupt flips then we are good
		new_elem.isFerroMag = false;
		QUEUE_enqueue(processQueue, new_elem);*/
	//}
}

// ISR for Ferro Sensor
ISR(INT3_vect)
{
	//LED_toggle(&led, 1);
	//if (FERRO_Read(&ferro))
	//{
		//QUEUE_BackPtr(processQueue)->isFerroMag = true;
	//}
}

// ISR for S2_OPTICAL
ISR(INT2_vect)
{
	
	if (OPTICAL_IsBlocked(&s2_optic)) //just saw falling edge
	{
		LED_toggle(&led, 2);
		//ADC_StartConversion(&adc);
	}/*
	else // just saw rising edge
	{ 
		//move item from the "process Queue" to the "ready Queue"
		if (!QUEUE_isEmpty(processQueue))
		{
			QueueElement processedItem = QUEUE_dequeue(processQueue);
			//add reflectivity for now -- calculate class here!
			//processedItem.reflectivity = Stage2.minReflectivity;
			QUEUE_enqueue(readyQueue, processedItem);
		}
	}*/
}

// ISR for EXIT_OPTICAL
ISR(INT0_vect)
{
	//LED_toggle(&led, 3);
	/*
	//if(OPTICAL_IsBlocked(&exit_optic))
	//{	
		if(!QUEUE_isEmpty(readyQueue))
		{
			QueueElement dropItem = QUEUE_dequeue(readyQueue);
			//PORTC = (uint8_t)((dropItem.reflectivity) >> 2);
			//uint8_t MSB1 = (uint8_t)((dropItem.reflectivity >> 9) << 7);
			//uint8_t MSB0 = (uint8_t)(((dropItem.reflectivity >> 8) & 0b00000001) << 5);
			//PORTD = (PORTD & 0x0F) | MSB1 | MSB0;
		}
	//}*/
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
	TIMER1_DelayMs(20);
	LED_toggle(&led, 6);
	TIMER1_DelayMs(20);
}

// ISR for PAUSE button
ISR(INT7_vect)
{
	// Debounce
	// We should probably set up a new different timer for this
	// Since this one will be used for the stepper motor
	TIMER1_DelayMs(20);
	LED_toggle(&led, 7);
	TIMER1_DelayMs(20);
}

ISR(ADC_vect)
{/*
	ADC_ReadConversion(&adc);
	
	if (adc.result < Stage2.minReflectivity)
		Stage2.minReflectivity = adc.result;
	
	if (OPTICAL_IsBlocked(&s2_optic))
		ADC_StartConversion(&adc);
	*/
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


