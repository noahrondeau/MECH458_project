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
	tray->delay = STEPPER_delayMax;
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
	
	if(dir == CW){ //CW rotation
		//check if direction change and change delay as check safe
		if(tray->lastDir == CCW) tray->delay = STEPPER_delayMax;
		//Step motor once CW
		STEPPER_StepCW(&(tray->stepper));
		//update current position
		tray->currentPos = (tray->currentPos + 1) % 200;
		//set last rotation direction to CW
		tray->lastDir = CW;
	}
	
	if(dir == CCW){ //CCW rotation
		//check if direction change and change delay as check safe
		if(tray->lastDir == CW) tray->delay = STEPPER_delayMax;
		//Step motor once CCW	
		STEPPER_StepCCW(&(tray->stepper));
		//update current position	
		if( tray->currentPos == 0 ) tray->currentPos = 199;
		else tray->currentPos--;
		//set last rotation direction to CW		
		tray->lastDir = CCW;
	}
}

void TRAY_Sort(Tray* tray){	

	// check the difference between the current position and target
	// and the target and turn as necessary
	int dist = (tray->targetPos) - (tray->currentPos);
	int dist_abs = abs(dist);
	

	
	if(dist == 0) //if tray is already in position
	{
		tray->beltPos = tray->targetPos;
		ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
		{
			tray->isReady = true;
		}
	}
	
	else if(dist_abs == 100) //180 deg away, starts the CW turn with 1 steps
	{
		TRAY_Rotate(tray,CW);
		TIMER1_DelayMs(TRAY_AccelDelay(tray));
	}
	//Check statements to determine direction of rotation
	else if( (dist_abs < 100 && dist > 0) || (dist_abs > 100 && dist < 0)) //CW rotation
	{
		TRAY_Rotate(tray,CW);
		TIMER1_DelayMs(TRAY_AccelDelay(tray));
	}
	else if((dist_abs < 100 && dist < 0) || (dist_abs > 100 && dist > 0)) //CCW Rotation
	{
		TRAY_Rotate(tray,CCW);
		TIMER1_DelayMs(TRAY_AccelDelay(tray));
	}
		
}


uint8_t TRAY_DistCalc(Tray* tray){
	return (uint8_t)(abs((&tray->targetPos)-(&tray->currentPos)));
}


//dist starts large, gets smaller as it rotates to target
uint8_t TRAY_AccelDelay(Tray* tray){
	int dist = TRAY_DistCalc(tray);
	int m_dist = (abs((&tray->targetPos)-(&tray->beltPos)));
	
	if(m_dist >= 2*STEPPER_accelRamp) //trap acceleration profile
	{
		if( (m_dist - dist) < STEPPER_accelRamp && tray->delay > STEPPER_delayMin) tray->delay--;
		if( (m_dist - dist) > (m_dist - STEPPER_accelRamp) && tray->delay < STEPPER_delayMax) tray->delay++;
	}
	else //triangle acceleration profile
	{
		if( (m_dist - dist) < (m_dist/2) && tray->delay > STEPPER_delayMin) tray->delay--;
		if( (m_dist - dist) > (m_dist/2) && tray->delay < STEPPER_delayMax) tray->delay++;
	}
	return tray->delay;
}

void TRAY_SetTarget(Tray* tray, uint8_t target)
{
	//if target is unclassified do nothing and return
	if ( target == UNCLASSIFIED )
	{
		ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
		{
			tray->isReady = true;
		}
		return;
	}
	
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