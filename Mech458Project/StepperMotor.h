/*
 * StepperMotor.h
 *
 * Created: 2018-11-13 11:35:22 PM
 *  Author: ntron
 */ 


#ifndef STEPPERMOTOR_H_
#define STEPPERMOTOR_H_



#include "config.h"

#define NUM_COMMUTATION_STEPS (4)
#define STEP0 (0b110000)
#define STEP1 (0b000110)
#define STEP2 (0b101000)
#define STEP3 (0b000101)


// @name:	StepperMotor
// @brief:	Represents a stepper motor
typedef struct StepperMotor
{
	unsigned char commutation_steps[NUM_COMMUTATION_STEPS];
	int step_index;
	int current_pos;
	int total_steps;
	GPIOReg port;
	GPIOReg ddr;
} StepperMotor;

// @name:	MotorDirection
// @brief:	describes direction of motor
typedef enum MotorDirection
{
	CW,
	CCW
} MotorDirection;

void STEPPER_init(StepperMotor* motor);

void STEPPER_step(	StepperMotor* motor,
					MotorDirection dir,
					int num_steps,
					int delay	);

void STEPPER_stepCW(StepperMotor* motor, int num_steps, int delay);
void STEPPER_stepCCW(StepperMotor* motor, int num_steps, int delay);


#endif /* STEPPERMOTOR_H_ */