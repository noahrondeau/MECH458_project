
#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdlib.h> 
#include "PWM.h"


void PWM_Init(unsigned char clockScale){

	//Clear OC0A on comp Match
	//Fast PWM 0xFF @ top, Update at TOP, TOV set on MAX
	TCCR0A |= 0b10000011;
	TCCR0A |= _BV(COM0A1);
	
	//Prescale Clock to clock prescaler -> Fclk = 125kHz
	TCCR0B |= clockScale;

	//DONT DO THIS!!!!!
	//Enables Timer/Counter 0 Match A & Overflow interrupts
	//TIMSK0 |= 0b00000011;
	
	DDRB |= (1<<7); 		//sets PB7 to output
}

void PWM_SetDutyCycle(unsigned char dutyCycle){
	OCR0A = dutyCycle;
}

void PWM_Stop()
{
	PWM_Set(0);
}