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
	tray->lastDelay = STEPPER_STARTUP_DELAY;
	STEPPER_Init(&(tray->stepper));
	HALL_Init(&(tray->hall));
}

void TRAY_Home(Tray* tray)
{
	
	if(HALL_IsActive(&(tray->hall))){
		for(int i = 0; i<25; i++)
		{
			STEPPER_StepCW(&(tray->stepper));
			TIMER1_DelayUs(STEPPER_DELAY_MAX);
		}
		while(!HALL_IsActive(&(tray->hall)))
		{
			STEPPER_StepCCW(&(tray->stepper));
			TIMER1_DelayUs(STEPPER_DELAY_MAX);	
		}
		STEPPER_StepCCW(&(tray->stepper));
		//tray->lastDir = CCW;
	}
	else
	{
		while(!HALL_IsActive(&(tray->hall)))
		{
			STEPPER_StepCW(&(tray->stepper));
			TIMER1_DelayUs(STEPPER_DELAY_MAX);
		}
		//tray->lastDir = CW;
	}
	
	tray->currentPos = 0;
	tray->beltPos = BLACK_PLASTIC;
}

void TRAY_Rotate(Tray* tray, MotorDirection dir){
	
	if(dir == CW){ //CW rotation
		//Step motor once CW
		STEPPER_StepCW(&(tray->stepper));
		
		//update current position
		tray->currentPos = (tray->currentPos + 1) % 200;
	}
	
	if(dir == CCW){ //CCW rotation
		//Step motor once CCW	
		STEPPER_StepCCW(&(tray->stepper));
		
		//update current position	
		if( tray->currentPos == 0 ) tray->currentPos = 199;
		else tray->currentPos--;
	}
}

void TRAY_Sort(Tray* tray){	

	// figure out shortest path to current target
	int shortest_path_dist = TRAY_CalcShortestPath(tray);
	
	//if tray is already in position, no turning to do, just signal that the tray is ready
	if(shortest_path_dist == 0)
	{
		ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
		{
			tray->beltPos = tray->targetPos;
			tray->lastDelay = STEPPER_STARTUP_DELAY;
			tray->isReady = true;
		}
		return; // early return in this case
	}
	
	// if the tray needs step, we need to calculate the required delay
	// THIS MUST BE DONE BEFORE TAKING THE STEP!!!!!
	
	uint16_t newDelay = TRAY_CalcStepDelay(tray, (uint16_t)abs(shortest_path_dist));
	// save the newDelay for use in the next step
	tray->lastDelay = newDelay;
	
	// now figure out the direction and take the step
	if(shortest_path_dist > 0) // we need to go CW, since the shortest path is positive
		TRAY_Rotate(tray, CW);
	else //if(shortest_path_dist < 0) // go CCW, since the shortest path is negative
		TRAY_Rotate(tray, CCW);
	// since we took care of the 0 case up top and returned from it, these are the only options
	
	// delay according to the calculated delay and exit
	TIMER1_DelayUs(newDelay);
}


int TRAY_CalcShortestPath(Tray* tray){
	
	int shortest_path_dist;
	
	// check the difference between the target and current position
	int dist = (tray->targetPos) - (tray->currentPos);
	
	// a positive distance corresponds to |dist| steps in the CW direction
	// e.g., 50 is 50 steps CW
	// e.g., 150 is 150 steps CW
	// however in the case that dist > 100, we want to turn CCW to get there
	
	// a negative distance corresponds to |dist| steps in the CCW direction
	// e.g. -50 is 50 steps CCW from the current position
	// e.g. -150 is 150 steps CCW from the current position.
	// however, we want to turn 50 steps clockwise in that case.
	
	// so we want to transform a dist >  100 -> -200 + dist, e.g.  120 -> -80
	// and transform a			 dist < -100 ->  200 + dist, e.g. -120 ->  80
	// a distance of magnitude 100 just becomes +100, because clockwise is our preferred direction

	if ( dist > 100 )
		shortest_path_dist = dist - 200;
	else if (dist < -100 )
		shortest_path_dist = dist + 200;
	else if (abs(dist) == 100) // for 100, we want this to be clockwise
		shortest_path_dist = 100;
	else // covers the |dist| < 100 case
		shortest_path_dist = dist;
		
	return shortest_path_dist;
}


//dist starts large, gets smaller as it rotates to target
uint16_t TRAY_CalcStepDelay(Tray* tray, uint16_t dist){
	
	uint16_t newDelay;
	if(MS_TO_US(dist) > STEPPER_ACCEL_RAMP)					// we haven't reached the deceleration zone
	{													// 1000 is to put on same order of magnitude
		if (tray->lastDelay > STEPPER_DELAY_MIN)			// we aren't going full speed yet
			newDelay = tray->lastDelay
						- STEPPER_MIN_DELAY_INCREMENT;				// make it faster
		else												// we are going at full speed
			newDelay = STEPPER_DELAY_MIN;							// keep it at full speed
	}
	else											// we are in the deceleration zone
	{	
		if(tray->lastDelay < STEPPER_DELAY_MAX)				// we aren't going at minimum speed yet
			newDelay = tray->lastDelay
						+ STEPPER_MIN_DELAY_INCREMENT;				// increase delay (decrease speed)
		else												// we are going at minimum speed
			newDelay = STEPPER_DELAY_MAX;							// keep it at minimum speed
	}
	
	return newDelay;
}

void TRAY_SetTarget(Tray* tray, uint8_t target)
{
	//if target is unclassified do nothing and return
	//if ( (target == UNCLASSIFIED) || (tray->targetPos == target))
	//{
	//	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	//	{
	//		tray->isReady = true;
	//	}
	//	return; // return if there is nothing to do
	//}
	
	if ( target != tray->targetPos) // new target and old target differ
	{
		ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
		{
			tray->targetPos = target;
			tray->isReady = false;
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