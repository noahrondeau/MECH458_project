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

void Tray_Rotate90(Tray* tray, MotorDirection dir){
	
}

void Tray_Rotate180(Tray* tray){
	for(int i = 0; i<100; i++)
	{
		STEPPER_StepCW(&(tray->stepper));
		TIMER1_DelayMs(20);
	}
}