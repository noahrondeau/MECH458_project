/*
 * StepperMotor.c
 *
 * Created: 2018-11-13 11:49:14 PM
 *  Author: ntron
 */ 


#include <avr/io.h>
#include "Timer.h"
#include "StepperMotor.h"

//Initialize the stepper
void STEPPER_Init(StepperMotor* motor)
{
	//rotating tray motor defs
	motor->commutation_steps[0] = STEP0;
	motor->commutation_steps[1] = STEP1;
	motor->commutation_steps[2] = STEP2;
	motor->commutation_steps[3] = STEP3;

	motor->step_index = 0;

	motor->ddrx = STEPPER_DDR;
	*(motor->ddrx) = 0xFF;

	motor->portx = STEPPER_PORT;
}

// to step, we give the stepper the correct order of pulses
// these are stored in the commutation step array
// the motor struct keeps track of which step it is on


void STEPPER_Step(	StepperMotor* motor,
					MotorDirection dir)
{
	if (dir == CW)
		STEPPER_StepCW(motor);
	else if (dir == CCW)
		STEPPER_StepCCW(motor);
}

void STEPPER_StepCW(StepperMotor* motor)
{
	motor->step_index++;
	if (motor->step_index == NUM_COMMUTATION_STEPS)
	motor->step_index = 0;

	*(motor->portx) = motor->commutation_steps[motor->step_index];
}

void STEPPER_StepCCW(StepperMotor* motor)
{
	motor->step_index--;
	if (motor->step_index < 0)
	motor->step_index = NUM_COMMUTATION_STEPS - 1;

	*(motor->portx) = motor->commutation_steps[motor->step_index];
}


