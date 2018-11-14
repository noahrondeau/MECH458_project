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
		case OPTICAL1:
		opt->pinx = OPTICAL1_PINX;
		opt->ddrx = OPTICAL1_DDRX;
		opt->mask = (uint8_t)((1) << OPTICAL1_PORTPIN);
		opt->active_level = ACTIVE_LOW;
		*(opt->ddrx) &= (~(opt->mask)); // set ddr as input for that pin

		EIMSK |= (_BV(INT0)); // enable INT1
		EICRA |= (_BV(ISC01));  // falling edge interrupt
		break;

		case OPTICAL2:
		opt->pinx = OPTICAL2_PINX;
		opt->ddrx = OPTICAL2_DDRX;
		opt->mask = (uint8_t)((1) << OPTICAL2_PORTPIN);
		opt->active_level = ACTIVE_HIGH;
		*(opt->ddrx) &= (~(opt->mask)); // set ddr as input for that pin

		EIMSK |= (_BV(INT1)); // enable INT2
		EICRA |= (_BV(ISC11)) | (_BV(ISC10)); // rising edge interrupt
		break;
	}

}

ISR(INT1_vect)
{

}

ISR(INT2_vect)
{

}