#ifndef TRAY_H_
#define TRAY_H_

#include "config.h"
#include "HallSensor.h"
#include "StepperMotor.h"
#include "Queue.h"


typedef volatile struct Tray
{
	uint8_t currentPos;
	uint16_t targetPos;
	uint16_t lastDelay;
	
	bool isReady;
	ItemClass beltPos;
	HallSensor hall;
	StepperMotor stepper;
	MotorDirection lastDir;
} Tray;

void TRAY_Init(Tray* tray);
void TRAY_Home(Tray* tray);
void TRAY_Rotate(Tray* tray, MotorDirection dir);
void TRAY_Sort(Tray* tray);

void TRAY_SetTarget(Tray* tray, uint8_t target);
uint8_t TRAY_GetTarget(Tray* tray);
uint8_t TRAY_GetCurrentPos(Tray* tray);
bool TRAY_IsReady(Tray* tray);
int TRAY_CalcShortestPath(Tray* tray);


#endif /* TRAY_H_ */