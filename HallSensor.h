#ifndef HALL_SENSOR_H
#define HALL_SENSOR_H

#include <avr/io.h>
#include <stdlib.h>
#include "config.h"

#define DDRD_REG ((GPIOReg)(0x0A + (__SFR_OFFSET)))
#define PIND_REG ((GPIOReg)(0x09 + (__SFR_OFFSET)))


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


#endif // HALL_SENSOR_H