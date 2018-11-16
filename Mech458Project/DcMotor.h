/*
 * DcMotor.h
 *
 * Created: 2018-11-13 11:24:17 PM
 *  Author: ntron
 */ 


#ifndef DCMOTOR_H_
#define DCMOTOR_H_

#include "config.h"
#include "PWM.h"

typedef volatile struct DcMotor
{
	GPIOReg portx;
	GPIOReg ddrx;
} DcMotor;

void DCMOTOR_Init(DcMotor* motor);
void DCMOTOR_Run(DcMotor* motor, unsigned char speed);
//void DCMOTOR_SetDirection(DcMotor* motor, MotorDirection dir);
void DCMOTOR_Brake(DcMotor* motor);



#endif /* DCMOTOR_H_ */