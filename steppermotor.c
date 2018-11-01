
#include <avr/io.h>
#include "timer.h"
#include "StepperMotor.h"


void STEPPER_init(StepperMotor* motor)
{
	//rotating tray motor defs
	motor->commutation_steps[0] = STEP0;
	motor->commutation_steps[1] = STEP1;
	motor->commutation_steps[2] = STEP2;
	motor->commutation_steps[3] = STEP3;

	motor->step_index = 0;
	motor->current_pos = 0;
	motor->total_steps = 200;

	motor->ddr = STEPPER_DDR;
	*(motor->ddr) = 0xFF;

	motor->port = STEPPER_PORT;
}

void STEPPER_step(	StepperMotor* motor,
					MotorDirection dir,
					int num_steps,
					int delay)
{
	if (dir == CW)
		STEPPER_stepCW(motor, num_steps, delay);
	else if (dir == CCW)
		STEPPER_stepCCW(motor,num_steps, delay);
}

void STEPPER_stepCW(StepperMotor* motor, int num_steps, int delay)
{
	for( int counter = 0; counter < num_steps; counter++ )
	{
		motor->step_index++;
		if (motor->step_index == NUM_COMMUTATION_STEPS)
			motor->step_index = 0;

		motor->current_pos++;
		if (motor->current_pos == motor->total_steps)
			motor->current_pos = 0;

		*(motor->port) = commutation_steps[step_index];
		TIMER_CountMs(delay);
	}
}

void STEPPER_stepCCW(StepperMotor* motor, int num_steps, int delay)
{
	for( int counter = 0; counter < num_steps; counter++ )
	{
		motor->step_index--;
		if (motor->step_index < 0)
			motor->step_index = NUM_COMMUTATION_STEPS - 1;

		motor->current_pos--;
		if (motor->current_pos < 0)
			motor->current_pos = motor->total_steps - 1;

		*(motor->port) = commutation_steps[step_index];
		TIMER_CountMs(delay);
	}
}


