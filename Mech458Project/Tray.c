/*
 * Tray.c
 *
 * Created: 2018-11-21 2:04:15 PM
 *  Author: ntron
 */ 

#include "Tray.h"
#include "StepperMotor.h"
#include "HallSensor.h"

void TRAY_Init(Tray* tray)
{
	tray->currentPos = 0;
	tray->targetPos = 0;
	STEPPER_Init(&(tray->stepper));
	HALL_Init(&(tray->hall));
}

void TRAY_Home(Tray* tray)
{
	
	
}