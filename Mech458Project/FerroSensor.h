/*
 * FerroSensor.h
 *
 * Created: 2018-11-13 11:28:27 PM
 *  Author: ntron
 */ 

/* Module to represent inductive sensor */

#ifndef FERROSENSOR_H_
#define FERROSENSOR_H_

#include <avr/io.h>
#include <avr/interrupt.h>
#include "config.h"

typedef volatile struct FerroSensor
{
	ActiveLevel active_level;
	GPIOReg pinx;
	GPIOReg ddrx;
	GPIOMask mask;
} FerroSensor;

void FERRO_Init(FerroSensor* ferro);
bool FERRO_Read(FerroSensor* ferro);


#endif /* FERROSENSOR_H_ */