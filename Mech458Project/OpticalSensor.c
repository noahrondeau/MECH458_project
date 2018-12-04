/*
* OpticalSensor.c
*
* Created: 2018-11-13 11:45:57 PM
*  Author: ntron
*/



#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>

#include "config.h"
#include "OpticalSensor.h"

void OPTICAL_Init(OpticalSensor* opt, OpticalSensorIdentity ident)
{
	switch(ident)
	{
	case S1_OPTICAL:
		opt->pinx = S1_OPTICAL_PINX;
		opt->ddrx = S1_OPTICAL_DDRX;
		opt->portpin = S1_OPTICAL_PORTPIN;
		opt->mask = (uint8_t)((1) << S1_OPTICAL_PORTPIN);
		opt->active_level = ACTIVE_LOW;
		*(opt->ddrx) &= (~(opt->mask)); // set ddr as input for that pin

		EIMSK |= (_BV(INT1)); // enable INT1
		EICRA |= (_BV(ISC11));// falling edge interrupt
		break;

	case S2_OPTICAL:
		opt->pinx = S2_OPTICAL_PINX;
		opt->ddrx = S2_OPTICAL_DDRX;
		opt->portpin = S2_OPTICAL_PORTPIN;
		opt->mask = (uint8_t)((1) << S2_OPTICAL_PORTPIN);
		opt->active_level = ACTIVE_HIGH;
		*(opt->ddrx) &= (~(opt->mask)); // set ddr as input for that pin

		EIMSK |= (_BV(INT2)); // enable INT1
		EICRA |= _BV(ISC21) | (_BV(ISC20)); // rising edge
		break;
		
	case EXIT_OPTICAL:
		opt->pinx = EXIT_OPTICAL_PINX;
		opt->ddrx = EXIT_OPTICAL_DDRX;
		opt->portpin = EXIT_OPTICAL_PORTPIN;
		opt->mask = (uint8_t)((1) << EXIT_OPTICAL_PORTPIN);
		opt->active_level = ACTIVE_LOW;
		*(opt->ddrx) &= (~(opt->mask)); // set ddr as input for that pin

		//	For the exit gate sensor
		EIMSK |= (_BV(INT0)); // enable INT2
		EICRA |= (_BV(ISC01)); // trigger falling edge
	}
}

bool OPTICAL_IsBlocked(OpticalSensor* opt)
{
	return ( (*(opt->pinx) & (opt->mask)) == ((opt->active_level) << (opt->portpin)) );
}