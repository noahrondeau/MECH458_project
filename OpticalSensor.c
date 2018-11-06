

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>

#include "config.h"

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
		break;

	case OPTICAL2:
		opt->pinx = OPTICAL2_PINX;
		opt->ddrx = OPTICAL2_DDRX;
		opt->mask = (uint8_t)((1) << OPTICAL2_PORTPIN);
		opt->active_level = ACTIVE_HIGH;
		*(opt->ddrx) &= (~(opt->mask)); // set ddr as input for that pin
		break;
	}

}