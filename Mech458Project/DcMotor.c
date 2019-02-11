/*
 * DcMotor.c
 *
 * Created: 2018-11-13 11:41:58 PM
 *  Author: ntron
 */ 


#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdlib.h>

#include "config.h"
#include "PWM.h"
#include "Timer.h"
#include "DcMotor.h"

// macros for simplifying readability

#define DO_DCMOTOR_SET_CW(__m__) \
*((__m__)->portx) = \
(*((__m__)->portx) & 0b10000000) | 0b00000010

#define DO_DCMOTOR_BRAKE(__m__) \
*((__m__)->portx) = ( *((__m__)->portx) & 0b10000000 )


#define DO_DCMOTOR_RUN(__m__) (DO_DCMOTOR_SET_CW(__m__))

// initialize motor
void DCMOTOR_Init(DcMotor* motor)
{
	motor->portx = DCMOTOR_PORTX;
	motor->ddrx = DCMOTOR_DDRX;
	*(motor->ddrx) = 0xFF;
	PWM_Init(Scale_64);
	motor->isRunning = false;
}

// start motor: unbrake and set PWM
void DCMOTOR_Run(DcMotor* motor, unsigned char speed)
{
	DO_DCMOTOR_RUN(motor);
	PWM_SetDutyCycle(speed);
	motor->isRunning = true;
}

// brake motor: brake to VCC and 0 duty cycle
void DCMOTOR_Brake(DcMotor* motor)
{
	DO_DCMOTOR_BRAKE(motor);
	PWM_SetDutyCycle(0);
	motor->isRunning = false;
}