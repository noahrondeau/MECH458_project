/*
 * StepperMotor.h
 *
 * Created: 2018-11-13 11:35:22 PM
 *  Author: ntron
 */ 

/* Module responsible for the business logic of stepping the motor */

#ifndef STEPPERMOTOR_H_
#define STEPPERMOTOR_H_

#include "config.h"

// there are two ways to step: single phase or 2 phase
// two phase is better, but we leave the option here (see config.h build modes)
#if MODE_ENABLED(TWO_PHASE_COMMUTATION_MODE)
	#define NUM_COMMUTATION_STEPS (4)
	#define STEP0 (0b110110)
	#define STEP1 (0b101110)
	#define STEP2 (0b101101)
	#define STEP3 (0b110101)
#else
	#define NUM_COMMUTATION_STEPS (4)
	#define STEP0 (0b110000)
	#define STEP1 (0b000110)
	#define STEP2 (0b101000)
	#define STEP3 (0b000101)
#endif

// @name:	StepperMotor
// @brief:	Represents a stepper motor
typedef volatile struct StepperMotor
{
	unsigned char commutation_steps[NUM_COMMUTATION_STEPS];
	int step_index;
	GPIOReg portx;
	GPIOReg ddrx;
} StepperMotor;

// @name:	MotorDirection
// @brief:	describes direction of motor
typedef enum MotorDirection
{
	CW,
	CCW
} MotorDirection;

void STEPPER_Init(StepperMotor* motor);

void STEPPER_Step(	StepperMotor* motor,
					MotorDirection dir);

void STEPPER_StepCW(StepperMotor* motor);
void STEPPER_StepCCW(StepperMotor* motor);


#endif /* STEPPERMOTOR_H_ */