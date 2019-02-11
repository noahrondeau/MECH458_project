/*
 * PushButton.h
 *
 * Created: 2018-11-19 7:23:52 PM
 *  Author: ntron
 */ 

/* Interface for the pushbuttons */

#ifndef PUSHBUTTON_H_
#define PUSHBUTTON_H_

#include "config.h"
#include <avr/io.h>
#include <avr/interrupt.h>

// enum representing which button we are dealing with
typedef enum PushButtonIdentity
{
	PAUSE_BUTTON,
	RAMPDOWN_BUTTON,
} PushButtonIdentity;

// struct representing pushbutton configs
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
void BUTTON_EnableInt();
void BUTTON_DisableInt();


#endif /* PUSHBUTTON_H_ */