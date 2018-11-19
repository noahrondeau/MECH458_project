

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
} Stage2;

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
	DCMOTOR_Run(&belt,50);
	
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
	
	// ====== INIT CODE END   ======
	sei(); // turn on interrupts
}


/* ====== INTERRUPT SERVICE ROUTINES ====== */

// ISR for S1_OPTICAL
ISR(INT0_vect)
{
	QueueElement new_elem = DEFAULT_QUEUE_ELEM;
	// increment total stat count and tag item with its count ID
	new_elem.counter = ++(ItemStats.totalCount);
	// initialize like this so that if no ferro interrupt flips then we are good
	new_elem.isFerroMag = false;
	QUEUE_enqueue(processQueue, new_elem);
}

// ISR for Ferro Sensor
ISR(INT6_vect)
{
	QUEUE_BackPtr(processQueue)->isFerroMag = true;
}

// ISR for S2_OPTICAL
ISR(INT1_vect)
{	
	if (OPTICAL_IsBlocked(&s2_optic)) //just saw falling edge
	{
		//ADC_StartConversion(&adc);
		
	}
	else // just saw rising edge
	{
		/* ADC reflectivity stuff go here!!!!! */
		
		//move item from the "process Queue" to the "ready Queue"
		if (!QUEUE_isEmpty(processQueue))
		{
			QueueElement processedItem = QUEUE_dequeue(processQueue);
			QUEUE_enqueue(readyQueue, processedItem);
		}
	}
}

// ISR for EXIT_OPTICAL
ISR(INT2_vect)
{
	if(!QUEUE_isEmpty(readyQueue))
	{
		QueueElement dropItem = QUEUE_dequeue(readyQueue);
		PORTC = (uint8_t)(dropItem.counter) | (((uint8_t)(dropItem.isFerroMag)) << 7);
	}
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
{
	ADC_ReadConversion(&adc);
	PORTC = (uint8_t)((adc.result) >> 2);
	
	if (OPTICAL_IsBlocked(&s2_optic))
		ADC_StartConversion(&adc);
	//else
		//PORTC = 0x00;
}



ISR(BADISR_vect)
{
	
}


