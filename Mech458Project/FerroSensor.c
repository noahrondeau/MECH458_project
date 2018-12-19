/*
 * FerroSensor.c
 *
 * Created: 2018-11-13 11:44:13 PM
 *  Author: ntron
 */ 



#include "FerroSensor.h"

// init sensor from config
void FERRO_Init(FerroSensor* ferro)
{
	ferro->pinx = FERRO_PINX;
	ferro->ddrx = FERRO_DDRX;
	ferro->mask = (uint8_t)((1) << FERRO_PORTPIN);
	ferro->active_level = ACTIVE_LOW;
	*(ferro->ddrx) &= (~(ferro->mask)); // set ddr as input for that pin
	
	EIMSK |= (_BV(INT5)); // enable INT5
	EICRB |= (_BV(ISC51));  // falling edge
}

// read sensor: mask out the PINX register and check it is same as the active level
bool FERRO_Read(FerroSensor* ferro)
{
	if ( (*(ferro->pinx) & (ferro->mask)) == ferro->active_level )
		return true;
	else
		return false;
}