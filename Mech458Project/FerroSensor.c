/*
 * FerroSensor.c
 *
 * Created: 2018-11-13 11:44:13 PM
 *  Author: ntron
 */ 



#include "FerroSensor.h"

void FERRO_Init(FerroSensor* ferro)
{
	ferro->pinx = FERRO_PINX;
	ferro->ddrx = FERRO_DDRX;
	ferro->mask = (uint8_t)((1) << FERRO_PORTPIN);
	ferro->active_level = ACTIVE_LOW;
	*(ferro->ddrx) &= (~(ferro->mask)); // set ddr as input for that pin
}

bool FERRO_Read(FerroSensor* ferro)
{
	if ( (*(ferro->pinx) & (ferro->mask)) == ferro->active_level )
	return true;
	else
	return false;
}