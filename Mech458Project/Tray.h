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
#include "Queue.h"


typedef volatile struct Tray
{
	int16_t currentPos;
	int16_t targetPos;
	ItemClass beltPos;
	HallSensor hall;
	StepperMotor stepper;
} Tray;

void TRAY_Init(Tray* tray);
void TRAY_Home(Tray* tray);

void TRAY_Rotate90(Tray* tray, MotorDirection dir);
void TRAY_Rotate180(Tray* tray);

void TRAY_AccelRotate90(Tray* tray, MotorDirection dir);
void TRAY_AccelRotate180(Tray* tray);

void TRAY_Sort(Tray* tray, ItemClass);

void TRAY_SetTarget(Tray* tray, int16_t target);
int16_t TRAY_GetTarget(Tray* tray);
int16_t TRAY_GetCurrentPos(Tray* tray);
bool TRAY_IsReady(Tray* tray);

#endif /* TRAY_H_ */