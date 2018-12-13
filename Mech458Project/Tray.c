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

#if MODE_ENABLED(ACCEL_MODE)
	// accel/deccel profile delay timings
	static volatile uint16_t delayProfile[STEPPER_ACCEL_RAMP] = DELAY_PROFILE_COEFFS; // see config.h for definition
	
#endif

void TRAY_Init(Tray* tray)
{
	tray->currentPos = 0;
	tray->targetPos = 0;
	tray->isReady = false;
	tray->beltPos = 0;
	STEPPER_Init(&(tray->stepper));
	HALL_Init(&(tray->hall));
}

// used to home the tray during system initialization
// if already over the hall effect sensor (position 0) then move a set amount to home the stepper
// otherwise turn all the way until we are in position 0
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
	}
	else
	{
		while(!HALL_IsActive(&(tray->hall)))
		{
			STEPPER_StepCW(&(tray->stepper));
			TIMER1_DelayUs(STEPPER_DELAY_MAX);
		}
	}
	
	tray->currentPos = 0;
}


// rotates the stepper in the direction known the to tray object
// called from TRAY_Process to rotate the tray
void TRAY_Rotate(Tray* tray, MotorDirection dir)
{
	
	if(dir == CW){ //CW rotation
		//Step motor once CW
		STEPPER_StepCW(&(tray->stepper));
		
		//update current position
		tray->currentPos = (tray->currentPos + 1) % 200;
	}
	else
	{ //CCW rotation
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
	
	for (int step = 0; step < abs(shortest_path_dist); step++)
	{
	
		// now figure out the direction and take the step
		if(shortest_path_dist > 0) // we need to go CW, since the shortest path is positive
			TRAY_Rotate(tray, CW);
		else //if(shortest_path_dist < 0) // go CCW, since the shortest path is negative
			TRAY_Rotate(tray, CCW);
		// since we took care of the 0 case up top and returned from it, these are the only options
		uint16_t nextDelay;
		if (step < STEPPER_ACCEL_RAMP) // need to accelerate
			nextDelay = delayProfile[step];
		else if (abs(shortest_path_dist) - step <= STEPPER_ACCEL_RAMP) // need to decelerate
			nextDelay = delayProfile[abs(shortest_path_dist) - step - 1 ];
		else // max speed
			nextDelay = STEPPER_DELAY_MIN;
		// delay according to the calculated delay and exit
		TIMER1_DelayUs(nextDelay);
	}
}

// calculates the shortest path to the target (distance + direction)
// called from TRAY_SetTarget
int TRAY_CalcShortestPath(Tray* tray){
	
	int shortest_path_dist;
	
	// check the difference between the target and current position
	int dist = (int)(tray->targetPos) - (int)(tray->currentPos);
	
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