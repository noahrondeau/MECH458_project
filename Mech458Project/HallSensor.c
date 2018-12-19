/*
 * HallSensor.c
 *
 * Created: 2018-11-13 11:44:58 PM
 *  Author: ntron
 */ 


#include <avr/io.h>
#include <stdlib.h>
#include "config.h"
#include "HallSensor.h"

// standard initialization according to the configuration
void HALL_Init(HallSensor* hall)
{
	hall->pinx = HALL_PINX;
	hall->ddrx = HALL_DDRX;
	hall->mask = (((uint8_t)(1)) << (HALL_PORTPIN) );
	hall->active_level = ACTIVE_LOW;

	*(hall->ddrx) &= (~(hall->mask)); // set ddr as input for that pin
}

// check if the Hall is active
// if the logic level is low,return true, else return false
bool HALL_IsActive(HallSensor* hall)
{
	//active low
	if (  ((*(hall->pinx)) & (hall->mask)) == ((hall->active_level) << HALL_PORTPIN) )
		return true;
	else
		return false;
}