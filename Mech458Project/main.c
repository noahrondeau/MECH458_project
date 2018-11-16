

/* ====== LIBRARY INCLUDES ====== */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>

/* ====== USER INCLUDES ======*/

#include "config.h"
#include "DcMotor.h"
#include "ADC.h"
#include "FerroSensor.h"
#include "HallSensor.h"
#include "OpticalSensor.h"
#include "Timer.h"

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

volatile unsigned char s2_edge_flag = 0;


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
	TIMER_DelayInit();
	DCMOTOR_Init(&belt);
	ADC_Init();
	FERRO_Init(&ferro);
	HALL_Init(&hall);
	OPTICAL_Init(&s1_optic,S1_OPTICAL);
	OPTICAL_Init(&s2_optic,S2_OPTICAL);
	OPTICAL_Init(&exit_optic,EXIT_OPTICAL);
	
	DDRC = 0xFF;
	PORTC = 0x00;
	
	// ====== INIT CODE END   ======
	sei(); // turn on interrupts
}


/* ====== INTERRUPT SERVICE ROUTINES ====== */

// ISR for S1_OPTICAL
ISR(INT0_vect)
{
	
}

// ISR for S2_OPTICAL
ISR(INT1_vect)
{
	PORTC = 0x00;
	
	if (OPTICAL_IsBlocked(&s2_optic)) //just saw falling edge
	{
		ADC_StartConversion(&adc);
	}
}

// ISR for EXIT_OPTICAL
ISR(INT2_vect)
{
	DCMOTOR_Brake(&belt);
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
// ISR for Ferro Sensor // UNUSED
ISR(INT6_vect)
{
	if(FERRO_Read(&ferro))
	{
		//PORTC = 0xff; // turn on LSB light
	}
}

ISR(ADC_vect)
{
	ADC_ReadConversion(&adc);
	PORTC = (uint8_t)((adc.result) >> 2);
	
	if (OPTICAL_IsBlocked(&s2_optic))
		ADC_StartConversion(&adc);
	else
		PORTC = 0x00;
}



ISR(BADISR_vect)
{
	
}


