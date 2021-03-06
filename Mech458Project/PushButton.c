/*
 * PushButton.c
 *
 * Created: 2018-11-19 7:24:06 PM
 *  Author: ntron
 */ 

#include "config.h"
#include "PushButton.h"
#include <avr/io.h>
#include <avr/interrupt.h>

// initialize the button according to configs and which button it is
void BUTTON_Init(PushButton* button, PushButtonIdentity ident)
{
	switch(ident)
	{
	case PAUSE_BUTTON:
		button->pinx = PAUSE_PINX;
		button->ddrx = PAUSE_DDRX;
		button->portpin = PAUSE_PORTPIN;
		button->mask = (uint8_t)((1) << PAUSE_PORTPIN);
		button->active_level = ACTIVE_LOW;
		*(button->ddrx) &= (~(button->mask)); // set ddr as input for that pin
		break;
		
	case RAMPDOWN_BUTTON:
		button->pinx = RAMPDOWN_PINX;
		button->ddrx = RAMPDOWN_DDRX;
		button->portpin = RAMPDOWN_PORTPIN;
		button->mask = (uint8_t)((1) << RAMPDOWN_PORTPIN);
		button->active_level = ACTIVE_LOW;
		*(button->ddrx) &= (~(button->mask)); // set ddr as input for that pin
		break;	
	}
}

// check if the button is pressed
bool BUTTON_IsPressed(PushButton* button)
{
	//active low
	if (  ((*(button->pinx)) & (button->mask))
			== ((button->active_level) << (button->portpin)) )
		return true;
	else
		return false;
}

// for the pushbuttons, we want to enable and disable
// interupts separately from initialization

void BUTTON_EnableInt()
{
	// set interrupts for falling edge pause button
	EIMSK |= (_BV(INT7)); // enable INT7
	EICRB |= (_BV(ISC71));  // falling edge
			
	// set interrupts for falling edge rampdown
	EIMSK |= (_BV(INT6)); // enable INT6
	EICRB |= (_BV(ISC61));  // falling edge
}

void BUTTON_DisableInt()
{
	// set interrupts for falling edge pause button
	EIMSK &= ~(_BV(INT7)); // enable INT7
	EICRB &= ~(_BV(ISC71));  // falling edge
		
	// set interrupts for falling edge rampdown
	EIMSK &= ~(_BV(INT6)); // enable INT6
	EICRB &= ~(_BV(ISC61));  // falling edge
}