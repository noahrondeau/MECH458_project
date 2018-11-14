/*
 * OpticalSensor.h
 *
 * Created: 2018-11-13 11:31:31 PM
 *  Author: ntron
 */ 


#ifndef OPTICALSENSOR_H_
#define OPTICALSENSOR_H_

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>

#include "config.h"

typedef enum OpticalSensorIdentity
{
	OPTICAL1,
	OPTICAL2,
} OpticalSensorIdentity;

typedef struct OpticalSensor
{
	ActiveLevel active_level;
	GPIOReg pinx;
	GPIOReg ddrx;
	GPIOMask mask;
} OpticalSensor;

void OPTICAL_Init(OpticalSensor* opt, OpticalSensorIdentity ident);



#endif /* OPTICALSENSOR_H_ */