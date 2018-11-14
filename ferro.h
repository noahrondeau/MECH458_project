#ifndef FERRO_H
#define FERRO_H

#include <avr/io.h>
#include <avr/interrupt.h>
#include "config.h"

typedef struct FerroSensor
{
	ActiveLevel active_level;
	GPIOReg pinx;
	GPIOReg ddrx;
	GPIOMask mask;
} FerroSensor;

void FERRO_Init(FerroSensor* ferro);
bool FERRO_Read(FerroSensor* ferro);

#endif // FERRO_H
