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
	uint8_t currentPos;
	uint8_t targetPos;
	uint8_t delay;
	uint8_t delayMax;
	uint8_t delayMin;
	
	bool isReady;
	ItemClass beltPos;
	HallSensor hall;
	StepperMotor stepper;
	MotorDirection lastDir;
} Tray;

void TRAY_Init(Tray* tray);
void TRAY_Home(Tray* tray);

void TRAY_Rotate90(Tray* tray, MotorDirection dir);
void TRAY_Rotate180(Tray* tray);

void TRAY_Rotate(Tray* tray, MotorDirection dir);

void TRAY_AccelRotate90(Tray* tray, MotorDirection dir);
void TRAY_AccelRotate180(Tray* tray);

void TRAY_Sort(Tray* tray);

void TRAY_SetTarget(Tray* tray, uint8_t target);
uint8_t TRAY_GetTarget(Tray* tray);
uint8_t TRAY_GetCurrentPos(Tray* tray);

bool TRAY_IsReady(Tray* tray);

uint8_t TRAY_DistCalc(Tray* tray);
uint8_t TRAY_AccelDelay(Tray* tray);


#endif /* TRAY_H_ */