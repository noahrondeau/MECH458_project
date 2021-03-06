/*
 * PWM.h
 *
 * Created: 2018-11-13 11:32:21 PM
 *  Author: ntron
 */ 

/* Enables PWM */

#ifndef PWM_H_
#define PWM_H_

// prescale for PWM
#define	Scale_0		(0x01) //CS00
#define	Scale_8 	(0x02) //CS01
#define	Scale_64 	(0x03) //CS01 & CS00
#define	Scale_256 	(0x04) //CS02
#define	Scale_1024 	(0x05) //CS02 & CS00

// right now only one timer is used for PWM,
// so no resource struct is required

void PWM_Init(unsigned char clockScale);
void PWM_SetDutyCycle(unsigned char dutyCycle);
void PWM_Stop();

#endif /* PWM_H_ */