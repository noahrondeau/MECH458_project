/*
 * Tray.c
 *
 * Created: 2018-11-21 2:04:15 PM
 *  Author: ntron
 */ 

#include "config.h"
#include "Tray.h"
#include "StepperMotor.h"
#include "HallSensor.h"
#include "Timer.h"

void TRAY_Init(Tray* tray)
{
	tray->currentPos = 0;
	tray->targetPos = 0;
	STEPPER_Init(&(tray->stepper));
	HALL_Init(&(tray->hall));
}

void TRAY_Home(Tray* tray)
{
	
	if(HALL_IsActive(&(tray->hall))){
		for(int i = 0; i<25; i++)
		{
			STEPPER_StepCW(&(tray->stepper));
			TIMER1_DelayMs(20);
		}
		while(!HALL_IsActive(&(tray->hall)))
		{
			STEPPER_StepCCW(&(tray->stepper));
			TIMER1_DelayMs(20);	
		}
	}
	else
	{
		while(!HALL_IsActive(&(tray->hall)))
		{
			STEPPER_StepCW(&(tray->stepper));
			TIMER1_DelayMs(20);
		}
	}
	
	tray->beltPos = BLACK_PLASTIC;
		
		
	
}

void TRAY_Rotate90(Tray* tray, MotorDirection dir){
	if(dir == CW){
		for(int i = 0; i<50; i++)
		{
			STEPPER_StepCW(&(tray->stepper));
			TIMER1_DelayMs(20);
		}
	}
	
	if(dir == CCW){
		for(int i = 0; i<50; i++)
		{
			STEPPER_StepCCW(&(tray->stepper));
			TIMER1_DelayMs(20);
		}
	}
}

void TRAY_Rotate180(Tray* tray){
	for(int i = 0; i<100; i++)
	{
		STEPPER_StepCW(&(tray->stepper));
		TIMER1_DelayMs(20);
	}
}

void TRAY_Sort(Tray* tray, QueueElement* q){
	tray->targetPos = q->class;
	
	switch ((tray->targetPos) - (tray->beltPos)){
		case 0:
			tray->beltPos = tray->targetPos;
			break;
		case 50:
			TRAY_Rotate90(&tray,CCW);
			tray->beltPos = tray->targetPos;
			break;
		case 100:
			TRAY_Rotate180(&tray);
			tray->beltPos = tray->targetPos;
			break;
		case 150:
			TRAY_Rotate90(&tray,CW);
			tray->beltPos = tray->targetPos;		
			break;
		case -50:
			TRAY_Rotate90(&tray,CW);
			tray->beltPos = tray->targetPos;
			break;
		case -100:
			TRAY_Rotate180(&tray);
			tray->beltPos = tray->targetPos;		
			break;
		case -150:
			TRAY_Rotate90(&tray,CCW);
			tray->beltPos = tray->targetPos;
			break;					
	}
	
}
