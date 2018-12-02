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
	uint8_t stepCounter;
	uint8_t pathDist;
	MotorDirection currDir;
	bool isReady;
	
	HallSensor hall;
	StepperMotor stepper;
} Tray;

void TRAY_Init(Tray* tray);
void TRAY_Home(Tray* tray);
void TRAY_Rotate(Tray* tray);
void TRAY_Sort(Tray* tray);

void TRAY_SetTarget(Tray* tray, uint8_t target);
uint8_t TRAY_GetTarget(Tray* tray);
uint8_t TRAY_GetCurrentPos(Tray* tray);
bool TRAY_IsReady(Tray* tray);
int TRAY_CalcShortestPath(Tray* tray);
uint16_t TRAY_CalcStepDelay(Tray* tray, uint16_t dist);


#endif /* TRAY_H_ */