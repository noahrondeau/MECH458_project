/*
 * LedBank.c
 *
 * Created: 2018-11-19 1:51:37 PM
 *  Author: ntron
 */ 

#include "LedBank.h"

// LEDs 8 and 9 are the two on-board LEDs connected to port D
// These functions make sure that the 9th and 10th bits (led 8 and 9)
// are set properly

void LED_Init()
{	
	DDRC |= 0xFF;
	PORTC = 0x00;
	
	DDRD |= 0xF0; // LOWER FOUR BITS OF DDRD MUST BE INPUTS (0)!!!
	PORTD &= 0x0F; // NEVER WRITE 1s to lower 4 bits of PORTD
}

// turns on an individual lED from 0 to 9
void LED_On(uint8_t led)
{
	if( led < 8 )// led is in lower 8 on port C
		PORTC |= (uint8_t)((uint8_t)(1) << led);
	else if ( led == 8 )
		PORTD |= (((uint8_t)(1)) << 5);
	else if ( led == 9 )
		PORTD |= (((uint8_t)(1)) << 7);
}

// turn off an individual LED from 0 to 9
void LED_Off(uint8_t led)
{
	if (led < 8 )
		PORTC &= ~((uint8_t)((uint8_t)(1) << led));
	else if ( led == 8 )
		PORTD &= ~(((uint8_t)(1)) << 5);
	else if (led == 9)
		PORTD &= ~(((uint8_t)(1)) << 7);
}

// toggle an individual LED from 0 to 9
void LED_Toggle(uint8_t led)
{
	if (led < 8)
		PORTC ^= (uint8_t)(((uint8_t)1) << led);
	else if (led == 8)
		PORTD ^= (((uint8_t)(1)) << 5);
	else if (led == 9)
		PORTD ^= (((uint8_t)(1)) << 7);
		
}

// set the entire 10 LED bank to a particular number
// the uupper 6 bits of input 16-bit field are ignored
void LED_Set(uint16_t seq)
{
	// set the lower byte to PORTC
	PORTC = (uint8_t)(seq & 0x00FF);
	
	// extract the bits for LED 8 and 9
	uint8_t MSB0 = ((uint8_t)((seq & 0xFF00) >> 8) & 0x01);
	uint8_t MSB1 = ((uint8_t)((seq & 0xFF00) >> 9) & 0x01);
	
	// shift the bits up to the right locations in PORTD for LEDs
	PORTD = (PORTD & 0x0F) | (MSB0 << 5) | (MSB1 << 7);
}

// only set the LEDs on PORTC
void LED_SetBottom8(uint8_t seq)
{
	PORTC = seq;
}