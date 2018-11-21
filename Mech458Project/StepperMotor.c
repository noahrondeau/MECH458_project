/*
 * StepperMotor.c
 *
 * Created: 2018-11-13 11:49:14 PM
 *  Author: ntron
 */ 


#include <avr/io.h>
#include "Timer.h"
#include "StepperMotor.h"


void STEPPER_init(StepperMotor* motor)
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

void STEPPER_step(	StepperMotor* motor,
MotorDirection dir)
{
	if (dir == CW)
	STEPPER_stepCW(motor);
	else if (dir == CCW)
	STEPPER_stepCCW(motor);
}

void STEPPER_stepCW(StepperMotor* motor)
{
	motor->step_index++;
	if (motor->step_index == NUM_COMMUTATION_STEPS)
	motor->step_index = 0;

	*(motor->portx) = motor->commutation_steps[motor->step_index];
}

void STEPPER_stepCCW(StepperMotor* motor)
{
	motor->step_index--;
	if (motor->step_index < 0)
	motor->step_index = NUM_COMMUTATION_STEPS - 1;

	*(motor->portx) = motor->commutation_steps[motor->step_index];
}


