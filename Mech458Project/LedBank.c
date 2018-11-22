/*
 * LedBank.c
 *
 * Created: 2018-11-19 1:51:37 PM
 *  Author: ntron
 */ 

#include "LedBank.h"

void LED_Init()
{	
	DDRC = 0xFF;
	PORTC = 0x00;
}

void LED_On(uint8_t led)
{
	PORTC |= (uint8_t)((uint8_t)(1) << led); 
}

void LED_Off(uint8_t led)
{
	PORTC &= ~((uint8_t)((uint8_t)(1) << led));
}

void LED_Toggle(uint8_t led)
{
	PORTC ^= (uint8_t)(((uint8_t)1) << led);
}

void LED_Set(uint8_t seq)
{
	PORTC = seq;	
}