/*
 * ExitSensor.h
 *
 * Created: 2018-11-13 11:27:17 PM
 *  Author: ntron
 */ 


#ifndef EXITSENSOR_H_
#define EXITSENSOR_H_

#include "config.h"
#include <avr/io.h>
#include <avr/interrupt.h>

typedef struct ExitSensor
{
	ActiveLevel active_level;
	GPIOReg pinx;
	GPIOReg ddrx;
	GPIOMask mask;
} ExitSensor;

void EXIT_Init(ExitSensor* ex);



#endif /* EXITSENSOR_H_ */