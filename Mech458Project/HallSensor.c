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

void HALL_Init(HallSensor* hall)
{
	hall->pinx = HALL_PINX;
	hall->ddrx = HALL_DDRX;
	hall->mask = (((uint8_t)(1)) << (HALL_PORTPIN) );

	*(hall->ddrx) &= (~(hall->mask)); // set ddr as input for that pin
}

bool HALL_isActive(HallSensor* hall)
{
	//active low
	if (  ((*(hall->pinx)) & (hall->mask)) == HALL_ACTIVE )
	return true;
	else
	return false;
}