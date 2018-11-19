/*
 * LedBank.c
 *
 * Created: 2018-11-19 1:51:37 PM
 *  Author: ntron
 */ 

#include "LedBank.h"

void LED_Init(LedBank* ledBank)
{
	for ( uint8_t i = 0; i < LED_BANK_LEN; i++)
	{
		ledBank->ledStatus[i] = false;
		ledBank->ledMask[i] = (uint8_t)((uint8_t)(1) << i); 
	}
	
	DDRC = 0xFF;
	PORTC = 0x00;
}

void LED_on(LedBank* ledBank, uint8_t led)
{
	ledBank->ledStatus[led] = true;
	PORTC |= ledBank->ledMask[led];
}

void LED_off(LedBank* ledBank, uint8_t led)
{
	ledBank->ledStatus[led] = false;
	PORTC &= ~(ledBank->ledMask[led]);
}

void LED_toggle(LedBank* ledBank, uint8_t led)
{
	if (ledBank->ledStatus[led] == false)
		LED_on(ledBank, led);
	else
		LED_off(ledBank, led);
}

void LED_set(LedBank* ledBank, uint8_t seq)
{
	for(uint8_t i = 0; i < LED_BANK_LEN; i++)
	{
		ledBank->ledStatus[i] = (0b00000001 & (seq >> i));
	}
	PORTC = seq;	
}