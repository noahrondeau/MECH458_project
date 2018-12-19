/*
 * HallSensor.h
 *
 * Created: 2018-11-13 11:29:36 PM
 *  Author: ntron
 */ 

/* Module implements reading from the Hall Sensor */

#ifndef HALLSENSOR_H_
#define HALLSENSOR_H_


#include <avr/io.h>
#include <stdlib.h>
#include "config.h"


typedef volatile struct HallSensor
{
	// There is only one stepper motor step
	// that will cause the HALL sensor to be active (ACTIVE LOW)
	GPIOReg pinx;
	GPIOReg ddrx;
	GPIOMask mask;
	ActiveLevel active_level;
} HallSensor;

void HALL_Init(HallSensor* hall);
bool HALL_IsActive(HallSensor* hall);


#endif /* HALLSENSOR_H_ */