/*
 * PushButton.h
 *
 * Created: 2018-11-19 7:23:52 PM
 *  Author: ntron
 */ 


#ifndef PUSHBUTTON_H_
#define PUSHBUTTON_H_

#include "config.h"
#include <avr/io.h>
#include <avr/interrupt.h>

typedef enum PushButtonIdentity
{
	PAUSE_BUTTON,
	RAMPDOWN_BUTTON,
} PushButtonIdentity;

typedef volatile struct PushButton
{
	ActiveLevel active_level;
	GPIOReg pinx;
	GPIOReg ddrx;
	GPIOMask mask;
	uint8_t portpin;
} PushButton;

void BUTTON_Init(PushButton* button, PushButtonIdentity ident);
bool BUTTON_IsPressed(PushButton* button);



#endif /* PUSHBUTTON_H_ */