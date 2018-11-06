#ifndef OPTICAL_SENSOR_H
#define OPTICAL_SENSOR_H

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>

#include "config.h"

typedef enum OpticalSensorIdentity
{
	OPTICAL1,
	OPTICAL2,
} OpticalSensorIdentity;

typedef enum bool
{
	false = 0,
	true = 1,
} bool;

typedef struct OpticalSensor
{
	ActiveLevel active_level;
	GPIOReg pinx;
	GPIOReg ddrx;
	GPIOMask mask;
} OpticalSensor;

void OPTICAL_Init(OpticalSensor* opt, OpticalSensorIdentity ident);


#endif // OPTICAL_SENSOR_H