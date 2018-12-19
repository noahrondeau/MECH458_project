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
	static volatile uint16_t delayProfile[STEPPER_ACCEL_RAMP] = DELAY_PROFILE_COEFFS;
	
#endif

// initialize the tray with default values
void TRAY_Init(Tray* tray)
{
	tray->currentPos = 0; // start in position 0
	tray->targetPos = 0; // start with target at current position
	tray->isReady = true;	// tray starts out ready
	tray->beltLock = false;	// we have not even begun a rotation, no belt lock yet
	tray->stepCounter = 0; // we haven't started moving, so not step counter
	tray->pathDist = 0; // there is no target, so there is not path distance to target yet
	tray->currDir = CW; // by default, we move CW because the motor prefers that
	tray->moveStartDelay = FIRST_ITEM_IN_QUEUE_DELAY; // this will be true for the first item
	STEPPER_Init(&(tray->stepper)); // initialize the stepper
	HALL_Init(&(tray->hall)); // initialize the hall sensor
}

// used to home the tray during system initialization
// if already over the hall effect sensor then move a set amount to home the stepper
// otherwise turn all the way until we are in position 0, then turn for offset
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
	
	// take steps counter-clockwise because the belt ejection zone is asymetric
	// we home to an offseted position from the Hall sensor
	for( int i = 0 ; i < TRAY_HOME_OFFSET; i++)
	{
		STEPPER_StepCCW(&(tray->stepper));
		TIMER1_DelayUs(STEPPER_DELAY_MAX);
	}
	
	// set the current pos
	tray->currentPos = 0;
}


// rotates the stepper in the direction known the to tray object
// called from TRAY_Process to rotate the tray
void TRAY_Rotate(Tray* tray)
{
	
	if(tray->currDir == CW){ //CW rotation
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

// get called from TIMER1_COMPA_vect ISR
// fetches the next delay, increments the internal tray state,
// rotates the tray, and schedules the next step
void TRAY_Process(Tray* tray){	
	
	// if tray is already in position, no turning to do
	// just signal that the tray is ready
	// do not start the interrupt again 
	if(tray->stepCounter == tray->pathDist)
	{
		ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
		{
			tray->stepCounter = 0;
			tray->pathDist = 0;
			tray->isReady = true;
		}
		return; // early return -- don't do this inside the atomic block
	}
		
	// give a step to the tray
	TRAY_Rotate(tray);
		
	// figure out next delay... do this before stepCounter is incremented!
	uint16_t nextDelay;
#if MODE_ENABLED(ACCEL_MODE)
	if (tray->stepCounter < STEPPER_ACCEL_RAMP) // need to accelerate
		nextDelay = delayProfile[tray->stepCounter];
	else if ((tray->pathDist - tray->stepCounter) <= STEPPER_ACCEL_RAMP) // need to decelerate
		nextDelay = delayProfile[tray->pathDist - tray->stepCounter - 1 ];
	else // max speed
		nextDelay = STEPPER_DELAY_MIN;
#else
	// if we aren't building in acceleration mode, just always make delay max
	nextDelay = STEPPER_DELAY_MAX;
#endif
	// increment step counter
	(tray->stepCounter)++;
		
	// schedule the next interrupt
	TIMER1_ScheduleIntUs(nextDelay);
	TIMER1_EnableInt();
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


// sets the tray target
// updates the tray's internal state including target, path distance to target, readiness and step count
// called in main to update the target when a new item is on the belt
void TRAY_SetTarget(Tray* tray, uint8_t target)
{
	if ( target != tray->targetPos) // new target and old target differ
	{	
		ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
		{
			tray->isReady = false;
			tray->targetPos = target;
			int shortest_path_dist = TRAY_CalcShortestPath(tray);
			tray->currDir = (shortest_path_dist >= 0) ? CW : CCW;
			tray->pathDist = abs(shortest_path_dist);
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

// check if the tray is at its target position
bool TRAY_IsReady(Tray* tray)
{
	bool ret;
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		ret = tray->isReady;
	}
	return ret;
}

// check if the tray is close enough to the target position that an item dropped from the belt
// would land comfortably in the quadrant
bool TRAY_inRange(Tray* tray)
{
	// return false if we are outside, in either direction
	if(tray->currDir == CW  && ((tray->pathDist - tray->stepCounter) > CW_Range ))
		return false;
	if(tray->currDir == CCW && ((tray->pathDist - tray->stepCounter) > CCW_Range))
		return false;
	
	// fall through is true
	return true;
}