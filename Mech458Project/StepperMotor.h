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