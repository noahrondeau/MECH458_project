/*
 * PWM.c
 *
 * Created: 2018-11-13 11:47:14 PM
 *  Author: ntron
 */ 


#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdlib.h>
#include "PWM.h"

// initialize with a particular clock prescaler
void PWM_Init(unsigned char clockScale){

	//Clear OC0A on comp Match
	//Fast PWM 0xFF @ top, Update at TOP, TOV set on MAX
	TCCR0A |= 0b10000011;
	TCCR0A |= _BV(COM0A1);
	
	//Prescale Clock to clock prescaler
	TCCR0B |= clockScale;
	
	DDRB |= (1<<7); 		//sets PB7 to output
	
	PWM_SetDutyCycle(0);// start off with no duty cycle
}

// set duty cycle
void PWM_SetDutyCycle(unsigned char dutyCycle){
	OCR0A = dutyCycle;
}

// stop the PWM by setting the duty cycle to 0
void PWM_Stop()
{
	PWM_SetDutyCycle(0);
}