/*
 * Tray.h
 *
 * Created: 2018-11-21 2:04:01 PM
 *  Author: ntron
 */ 


#ifndef TRAY_H_
#define TRAY_H_

#include "config.h"
#include "HallSensor.h"
#include "StepperMotor.h"

typedef volatile struct Tray
{
	uint8_t currentPos;
	uint8_t targetPos;
	HallSensor hall;
	StepperMotor stepper;
} Tray;

void TRAY_Init(Tray* tray);
void TRAY_Home(Tray* tray);

#endif /* TRAY_H_ */