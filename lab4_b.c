#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdlib.h>
#include "lab2.h"
#include "PWM.h"

// define the global variables that can be used in every function ==========
volatile unsigned char ADC_result;
volatile unsigned int ADC_result_flag;
void ADC_Init(void);

#define IS_MOTOR_CW  ((PORTB & 0b00000011) == 0b00000010)
#define IS_MOTOR_CCW ((PORTB & 0b00000011) == 0b00000001)

#define DO_MOTOR_CW  ((PORTB & 0b10000000) | 0b00000010 )
#define DO_MOTOR_CCW ((PORTB & 0b10000000) | 0b00000001 )

#define DO_MOTOR_BRAKE ( PORTB & 0b10000000 )

#define FALSE (0)
#define TRUE (1)

volatile int button_pressed = FALSE;

int main()
{
	DDRA = 0x00;
	DDRB = 0xff;
	DDRC = 0xff;
	DDRF = 0x00;
	PORTB = PORTB & 0x00;
	
	TIMER_Init();
	ADC_Init();
	PWM_Init(Scale_8);
	
	TIFR0 |= _BV(OCF0A); // Clear interrupt flag
	sei();
	ADCSRA |= _BV(ADSC);
	
	
	//set clockwise
	PORTB = DO_MOTOR_CW;

	while (1)
	{
		if (ADC_result_flag)
		{
			PORTC = ADC_result;
			PWM_Set(ADC_result);
			ADC_result_flag = 0x00;
			ADCSRA |= _BV(ADSC);
		}

		if (button_pressed == TRUE)
		{
			if( IS_MOTOR_CW ){	//Clockwise

				PORTB = DO_MOTOR_BRAKE;
				TIMER_CountMs(5);
				PORTB = DO_MOTOR_CCW;

			}else if (IS_MOTOR_CCW ) {	//Counter Clockwise

				PORTB = DO_MOTOR_BRAKE;
				TIMER_CountMs(5);
				PORTB = DO_MOTOR_CW;
			}

			button_pressed = FALSE;
		}
	}

	return 0;
}


void ADC_Init(void){
	
	cli(); // disable all of the interrupt ==========================

	EIMSK |= (_BV(INT0)); // enable INT0
	EICRA |= 0b00000010;//(_BV(ISC01)); // | _BV(ISC00)); // falling edge interrupt
	/*
	// config the external interrupt ======================================
	EIMSK |= (_BV(INT2)); // enable INT2
	EICRA |= (_BV(ISC21) | _BV(ISC20)); // rising edge interrupt*/

	// config ADC =========================================================
	// by default, the ADC input (analog input is set to be ADC0 / PORTF0
	ADCSRA |= _BV(ADEN); // enable ADC
	ADCSRA |= _BV(ADIE); // enable interrupt of ADC
	//sets ADLAR 1 which makes ADCH 8bits read 1st ADCL 2 bits read 2nd
	ADMUX |= _BV(ADLAR) | _BV(REFS0);

	// set the PORTA as output to display the ADC result ==================
	DDRA = 0xff;

	// sets the Global Enable for all interrupts ==========================
	sei();

	// initialize the ADC, start one conversion at the beginning ==========
	ADCSRA |= _BV(ADSC);
}

/*****************ISR******************************/
ISR(INT0_vect){

	if (button_pressed == FALSE)
	{
		TIMER_CountMs(20);
		PORTC = 0x01;
		while((PIND&0x01) == 0);
		PORTC = 0;
		button_pressed = TRUE;
	}
}

// sensor 3: 2nd Optical Inductive, Active HIGH starts AD converstion =======
ISR(INT2_vect)
{
	// when there is a rising edge, we need to do ADC =====================
	ADCSRA |= _BV(ADSC);
}

// the interrupt will be trigured if the ADC is done ========================
ISR(ADC_vect)
{
	ADC_result = ADCH;
	ADC_result_flag = 1;
}
