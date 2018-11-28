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
	tray->isReady = false;
	tray->delay = 20;
	tray->delayMax = 20;
	tray->delayMin = 8;
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
		tray->lastDir = CCW;
	}
	else
	{
		while(!HALL_IsActive(&(tray->hall)))
		{
			STEPPER_StepCW(&(tray->stepper));
			TIMER1_DelayMs(20);
		}
		tray->lastDir = CW;
	}
	
	tray->currentPos = 0;
	tray->beltPos = BLACK_PLASTIC;
}

void TRAY_Rotate(Tray* tray, MotorDirection dir){
	if(dir == CW){
		if(tray->lastDir == CCW) tray->delay = tray->delayMax;
		STEPPER_StepCW(&(tray->stepper));
		tray->currentPos = (tray->currentPos + 1) % 200;
		tray->lastDir = CW;
	}
	
	if(dir == CCW){
		if(tray->lastDir == CW) tray->delay = tray->delayMax;
		STEPPER_StepCCW(&(tray->stepper));		
		if( tray->currentPos == 0 ) tray->currentPos = 199;
		else tray->currentPos--;
		tray->lastDir = CCW;
	}
}

void TRAY_Sort(Tray* tray, ItemClass class){
	
	TRAY_SetTarget(tray, class);
	
	// if its an unclassified item don't do anything
	if ( TRAY_GetTarget(tray) == UNCLASSIFIED )
		return;
	
	// check the difference between the current position
	// and the target and turn as necessary
	int dist = (tray->targetPos) - (tray->currentPos);
	
	if(dist == 0)
	{
		tray->beltPos = tray->targetPos;
		ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
		{
			tray->isReady = true;
		}
	}
	else if(dist == 100 || dist == -100)
	{
		TRAY_Rotate(tray,CW);
		TIMER1_DelayMs(tray->delayMax);
	}
	else if(dist == 50 || dist == 100 || dist == -100 || dist == -150)
	{
		TRAY_Rotate(tray,CW);
		TIMER1_DelayMs(TRAY_AccelDelay(tray));
	}
	else if(dist == 150 || dist == -50)
	{
		TRAY_Rotate(tray,CCW);
		TIMER1_DelayMs(TRAY_AccelDelay(tray));
	}
	
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		tray->isReady = false;
	}
	
	
}


uint8_t TRAY_DistCalc(Tray* tray){
	return (uint8_t)(abs((&tray->targetPos)-(&tray->currentPos)));
}

uint8_t TRAY_AccelDelay(Tray* tray){
	int dist = TRAY_DistCalc(tray);
	if(dist<12 && tray->delay > tray->delayMin) tray->delay--;
	if(12 <= dist && dist < (tray->targetPos - 11)) tray->delay;
	if((tray->targetPos - 11) <= dist && dist<(tray->targetPos) && tray->delay < tray->delayMax) tray->delay++;
	
	return tray->delay;
}

void TRAY_SetTarget(Tray* tray, uint8_t target)
{
	if ( target != tray->targetPos) // new target and old target differ
	{
		ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
		{
			tray->targetPos = target;
			tray->isReady = false;
		}
	}
	else
	{
		ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
		{
			tray->isReady = true;
		}
	}
}

uint8_t TRAY_GetTarget(Tray* tray)
{
	uint8_t ret;
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		ret = tray->targetPos;
	}
	return ret;
}

uint8_t TRAY_GetCurrentPos(Tray* tray)
{
	uint8_t ret;
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		ret = tray->currentPos;
	}
	return ret;
}

bool TRAY_IsReady(Tray* tray)
{
	bool ret;
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		ret = tray->isReady;
	}
	return ret;
}

//Legacy

/*
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

void TRAY_AccelRotate90(Tray* tray, MotorDirection dir){
	if(dir == CW){
		STEPPER_StepCW(&(tray->stepper));
		TIMER2_DelayUs(TRAY_AccelDelay(tray)*1000);
		tray->currentPos= (tray->currentPos + 1) % 200;
	}
	
	
	if(dir == CCW){
		STEPPER_StepCCW(&(tray->stepper));
		TIMER2_DelayUs(TRAY_AccelDelay(tray)*1000);
		if( tray->currentPos == 0 )
		tray->currentPos = 199;
		else
		tray->currentPos--;
	}
}

void TRAY_AccelRotate180(Tray* tray){
	STEPPER_StepCW(&(tray->stepper));
	TIMER2_DelayUs(TRAY_AccelDelay(tray)*1000);
	tray->currentPos= (tray->currentPos + 1) % 200;
}

*/