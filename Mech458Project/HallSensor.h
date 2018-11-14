/*
 * HallSensor.h
 *
 * Created: 2018-11-13 11:29:36 PM
 *  Author: ntron
 */ 


#ifndef HALLSENSOR_H_
#define HALLSENSOR_H_


#include <avr/io.h>
#include <stdlib.h>
#include "config.h"


typedef struct HallSensor
{
	// There is only one stepper motor step
	// that will cause the HALL sensor to be active (ACTIVE LOW)
	GPIOReg pinx;
	GPIOReg ddrx;
	GPIOMask mask;
} HallSensor;

void HALL_Init(HallSensor* hall);
bool HALL_isActive(HallSensor* hall);


#endif /* HALLSENSOR_H_ */