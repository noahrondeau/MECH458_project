

/* ====== LIBRARY INCLUDES ====== */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>

/* ====== USER INCLUDES ======*/

#include "config.h"
#include "SysClk.h"
#include "DcMotor.h"
#include "ADC.h"
#include "FerroSensor.h"
#include "HallSensor.h"
#include "OpticalSensor.h"
#include "Timer.h"
#include "Queue.h"

/* ====== MAIN-LOCAL DEFINITIONS ====== */

/* ====== FUNCTION PROTOTYPES ====== */
void Initialize();

/* ====== GLOBAL SYSTEM RESOURCES ====== */

volatile DcMotor		belt;
volatile ADCHandle		adc;
volatile FerroSensor	ferro;
volatile HallSensor		hall;
volatile OpticalSensor	s1_optic;
volatile OpticalSensor	s2_optic;
volatile OpticalSensor	exit_optic;

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
	TIMER_DelayMs(2000);
	DCMOTOR_Run(&belt,100);
	
	while(1)
	{
		
	}
	return 0;
}

void Initialize()
{
	cli(); // turn off interrupts
	// ====== INIT CODE START ======
	Sys_Clk_Init();
	TIMER_DelayInit();
	DCMOTOR_Init(&belt);
	ADC_Init();
	FERRO_Init(&ferro);
	HALL_Init(&hall);
	OPTICAL_Init(&s1_optic,S1_OPTICAL);
	OPTICAL_Init(&s2_optic,S2_OPTICAL);
	OPTICAL_Init(&exit_optic,EXIT_OPTICAL);
	
	readyQueue = QUEUE_create();
	processQueue = QUEUE_create();
	
	DDRC = 0xFF;
	PORTC = 0x00;
	
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
		if ((PORTC & 0b00000001) == 0b00000000) PORTC |= 0b00000001;
		else PORTC = PORTC & 0b11111110;
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
ISR(INT6_vect)
{
	//if (FERRO_Read(&ferro))
	//{
		if ((PORTC & 0b00000010) == 0b00000000) PORTC |= 0b00000010;
		else PORTC = PORTC & 0b11111101;
		//QUEUE_BackPtr(processQueue)->isFerroMag = true;
	//}
}

// ISR for S2_OPTICAL
ISR(INT2_vect)
{/*
	if (OPTICAL_IsBlocked(&s2_optic)) //just saw falling edge
	{
		//ADC_StartConversion(&adc);
	}
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
{/*
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
// ISR for PAUSE BUTTON (SW1) //UNUSED
ISR(INT3_vect)
{
	
}

// ISR for RAMP_DOWN BUTTON (SW2) //UNUSED
ISR(INT4_vect)
{
	
}

// ISR for Hall Sensor	//UNUSED
ISR(INT5_vect)
{
	
}
*/

ISR(ADC_vect)
{/*
	ADC_ReadConversion(&adc);
	
	if (adc.result < Stage2.minReflectivity)
		Stage2.minReflectivity = adc.result;
	
	if (OPTICAL_IsBlocked(&s2_optic))
		ADC_StartConversion(&adc);
	*/
}



ISR(BADISR_vect)
{
	while(1)
	{
		PORTC = 0b01010101;
		TIMER_DelayMs(500);
		PORTC = 0b10101010;
		TIMER_DelayMs(500);
	}
}


