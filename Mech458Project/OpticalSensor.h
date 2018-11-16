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
	S1_OPTICAL,
	S2_OPTICAL,
	EXIT_OPTICAL,
} OpticalSensorIdentity;

typedef volatile struct OpticalSensor
{
	ActiveLevel active_level;
	GPIOReg pinx;
	GPIOReg ddrx;
	GPIOMask mask;
	uint8_t portpin;
} OpticalSensor;

void OPTICAL_Init(OpticalSensor* opt, OpticalSensorIdentity ident);
bool OPTICAL_IsBlocked(OpticalSensor* opt);



#endif /* OPTICALSENSOR_H_ */