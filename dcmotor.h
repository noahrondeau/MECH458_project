
#ifndef DC_MOTOR_H
#define DC_MOTOR_H

#include "config.h"
#include "motor.h"
#include "PWM.h"

typedef struct DcMotor
{
	GPIOPort port;
	GPIOPort ddr;
	PWM pwm;
} DcMotor;

void DCMOTOR_Init(DcMotor* motor);
void DCMOTOR_Run(DcMotor* motor, unsigned char speed);
//void DCMOTOR_SetDirection(DcMotor* motor, MotorDirection dir);
void DCMOTOR_Brake(DcMotor* motor);

#endif