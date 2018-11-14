
#ifndef EXIT_SENSOR_H
#define EXIT_SENSOR_H

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

#endif