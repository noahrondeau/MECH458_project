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

// we don't really need all these, just one
// since we aren't actually changing direction

#define IS_DCMOTOR_CW(__m__) \
((*((__m__)->portx) & 0b00000011) == 0b00000010)
#define IS_DCMOTOR_CCW(__m__) \
(( *((__m__)->portx) & 0b00000011) == 0b00000001)

#define DO_DCMOTOR_SET_CW(__m__) \
*((__m__)->portx) = \
(*((__m__)->portx) & 0b10000000) | 0b00000010

#define DO_DCMOTOR_SET_CCW(__m__) \
*((__m__)->portx) = \
(*((__m__)->portx) & 0b10000000) | 0b00000010

#define DO_DCMOTOR_BRAKE(__m__) \
*((__m__)->portx) = ( *((__m__)->portx) & 0b10000000 )


// this is the only one we need
#define DO_DCMOTOR_RUN(__m__) (DO_DCMOTOR_SET_CW(__m__))


void DCMOTOR_Init(DcMotor* motor)
{
	motor->portx = DCMOTOR_PORTX;
	motor->ddrx = DCMOTOR_DDRX;
	*(motor->ddrx) = 0xFF;
	PWM_Init(Scale_8);

}

void DCMOTOR_Run(DcMotor* motor, unsigned char speed)
{
	PWM_SetDutyCycle(speed);
	DO_DCMOTOR_RUN(motor);
}

void DCMOTOR_Brake(DcMotor* motor)
{
	DO_DCMOTOR_BRAKE(motor);
}