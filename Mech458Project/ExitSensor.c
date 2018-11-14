/*
 * ExitSensor.c
 *
 * Created: 2018-11-13 11:43:29 PM
 *  Author: ntron
 */ 


#include "ExitSensor.h"
#include <avr/io.h>
#include <avr/interrupt.h>

void EXIT_Init(ExitSensor* ex)
{
	ex->pinx = EXIT_PINX;
	ex->ddrx = EXIT_DDRX;
	ex->mask = (uint8_t)((1) << EXIT_PORTPIN);
	ex->active_level = ACTIVE_LOW;
	*(ex->ddrx) &= (~(ex->mask)); // set ddr as input for that pin

	//	For the exit gate sensor
	EIMSK |= (_BV(INT2)); // enable INT2
	EICRA |= (_BV(ISC21)); // trigger falling edge
}