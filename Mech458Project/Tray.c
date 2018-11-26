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
#include "LedBank.h"

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
		STEPPER_StepCCW(&(tray->stepper));
	}
	else
	{
		while(!HALL_IsActive(&(tray->hall)))
		{
			STEPPER_StepCW(&(tray->stepper));
			TIMER1_DelayMs(20);
		}
	}
	
	tray->currentPos = 0;
	tray->beltPos = BLACK_PLASTIC;
		
		
	
}

void TRAY_Rotate90(Tray* tray, MotorDirection dir){
	if(dir == CW){
		for(int i = 0; i<50; i++)
		{
			STEPPER_StepCW(&(tray->stepper));
			tray->currentPos = (tray->currentPos + 1) % 200;
			TIMER1_DelayMs(20);
		}
	}
	
	if(dir == CCW){
		for(int i = 0; i<50; i++)
		{
			STEPPER_StepCCW(&(tray->stepper));
			
			if( tray->currentPos == 0 )
				tray->currentPos = 199;
			else
				tray->currentPos--;
			
			TIMER1_DelayMs(20);
		}
	}
}

void TRAY_Rotate180(Tray* tray){
	for(int i = 0; i<100; i++)
	{
		STEPPER_StepCW(&(tray->stepper));
		tray->currentPos = (tray->currentPos + 1) % 200;
		TIMER1_DelayMs(20);
	}
}

void TRAY_Sort(Tray* tray, ItemClass class){
	
	TRAY_SetTarget(tray, (int16_t)class );
	
	// if its an unclassified item don't do anything
	if ( TRAY_GetTarget(tray) == UNCLASSIFIED )
		return;
	
	// check the difference between the current position
	// and the target and turn as necessary
	switch ((tray->targetPos) - (tray->beltPos)){
		case (0):
		{
			tray->beltPos = tray->targetPos;
		}
			break;
		case (50):
		{
			TRAY_Rotate90(tray,CCW);
			tray->beltPos = tray->targetPos;
		}
			break;
		case (100):
		{
			TRAY_Rotate180(tray);
			tray->beltPos = tray->targetPos;
			break;
		}
		case (150):
		{
			TRAY_Rotate90(tray,CW);
			tray->beltPos = tray->targetPos;		
			break;
		}
		case (-50):
		{
			TRAY_Rotate90(tray,CW);
			tray->beltPos = tray->targetPos;
			break;
		}
		case (-100):
		{
			TRAY_Rotate180(tray);
			tray->beltPos = tray->targetPos;
		}
			break;
		case (-150):
		{
			TRAY_Rotate90(tray,CCW);
			tray->beltPos = tray->targetPos;
		}
			break;
		default: //should never reach here
			break;			
	}
	
}

void TRAY_AccelRotate90(Tray* tray, MotorDirection dir){
	if(dir == CW){
		//0-11
		for(int i=0; i<12; i++)
		{
			STEPPER_StepCW(&(tray->stepper));
			TIMER1_DelayMs(20-1*i);
			tray->currentPos= (tray->currentPos + 1) % 200;
		}
		//12-37
		for(int i=0;  i<26; i++)
		{
			STEPPER_StepCW(&(tray->stepper));
			TIMER1_DelayMs(8);
			tray->currentPos= (tray->currentPos + 1) % 200;
		}
		//38-49
		for(int i=0;  i<12; i++)
		{
			STEPPER_StepCW(&(tray->stepper));
			TIMER1_DelayMs(8+1*i);
			tray->currentPos= (tray->currentPos + 1) % 200;
		}
	}
	
	
	if(dir == CCW){
		//0-11
		for(int i=0; i<12; i++)
		{
			STEPPER_StepCCW(&(tray->stepper));
			TIMER1_DelayMs(20-1*i);
			tray->currentPos= (tray->currentPos + 1) % 200;
		}
		//12-37
		for(int i=0;  i<26; i++)
		{
			STEPPER_StepCCW(&(tray->stepper));
			TIMER1_DelayMs(8);
			tray->currentPos= (tray->currentPos + 1) % 200;
		}
		//38-49
		for(int i=0;  i<12; i++)
		{
			STEPPER_StepCCW(&(tray->stepper));
			TIMER1_DelayMs(8+1*i);
			tray->currentPos= (tray->currentPos + 1) % 200;
		}
	}
}

void TRAY_AccelRotate180(Tray* tray){
	//0-11
	for(int i=0; i<12; i++)
	{
		STEPPER_StepCW(&(tray->stepper));
		TIMER1_DelayMs(20-1*i);
		tray->currentPos= (tray->currentPos + 1) % 200;
	}
	//12-87
	for(int i=0;  i<76; i++)
	{
		STEPPER_StepCW(&(tray->stepper));
		TIMER1_DelayMs(8);
		tray->currentPos= (tray->currentPos + 1) % 200;
	}
	//88-99
	for(int i=0;  i<12; i++)
	{
		STEPPER_StepCW(&(tray->stepper));
		TIMER1_DelayMs(8+1*i);
		tray->currentPos= (tray->currentPos + 1) % 200;
	}
}


void TRAY_SetTarget(Tray* tray, int16_t target)
{
	tray->targetPos = target;
}

int16_t TRAY_GetTarget(Tray* tray)
{
	return tray->targetPos;
}

int16_t TRAY_GetCurrentPos(Tray* tray)
{
	 return tray->currentPos;	
}

bool TRAY_IsReady(Tray* tray)
{
	return (TRAY_GetCurrentPos(tray) == TRAY_GetTarget(tray));
}
