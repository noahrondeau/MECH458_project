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
		
		// set interrupts for falling edge
		EIMSK |= (_BV(INT7)); // enable INT7
		EICRA |= (_BV(ISC71));  // falling edge
		break;
		
	case RAMPDOWN_BUTTON:
		button->pinx = RAMPDOWN_PINX;
		button->ddrx = RAMPDOWN_DDRX;
		button->portpin = RAMPDOWN_PORTPIN;
		button->mask = (uint8_t)((1) << RAMPDOWN_PORTPIN);
		button->active_level = ACTIVE_LOW;
		*(button->ddrx) &= (~(button->mask)); // set ddr as input for that pin
		
		// set interrupts for falling edge
		EIMSK |= (_BV(INT6)); // enable INT6
		EICRA |= (_BV(ISC61));  // falling edge
		break;	
	}
}


bool BUTTON_IsPressed(PushButton* button)
{
	//active low
	if (  ((*(button->pinx)) & (button->mask))
			== ((button->active_level) << (button->portpin)) )
		return true;
	else
		return false;
}