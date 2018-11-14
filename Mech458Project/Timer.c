/*
 * Timer.c
 *
 * Created: 2018-11-13 11:49:43 PM
 *  Author: ntron
 */ 



#include <avr/io.h>
#include <avr/interrupt.h>
#include "Timer.h"

#define TIMER_COUNT_REACHED (0x02)
#define LED_DIR_LEFT (0)
#define LED_DIR_RIGHT (1)
#define LED_RIGHT_END (0b00000011)
#define LED_LEFT_END (0b11000000)

void TIMER_Init(void)
{
	TCCR1B |= _BV(CS10);
}

void TIMER_CountMs(int ms)
{
	// Index for loop
	int i = 0;
	// Set Waveform Gen Mode to CTC mode
	TCCR1B |= _BV(WGM12);
	// Set output compare register to count 1000 cycles = 1ms
	OCR1A = 0x03e8;
	// Reset the timer
	TCNT1 = 0x0000;

	//DONT DO THIS!!! we dont want interrupt
	// Enable the output compare interrupt enable
	//TIMSK1 = TIMSK1 | 0b00000010;
	// clear interrupt flag and begin counting
	TIFR1 |= _BV(OCF1A);

	// loop "ms" times, each loop iteration is one millisecond
	while (i < ms)
	{
		// wait for the timer to be reached
		while ((TIFR1 & TIMER_COUNT_REACHED) != TIMER_COUNT_REACHED);
		// increment loop counter
		i++;
		// clear interrupt flag and begin counting
		TIFR1 |= _BV(OCF1A);
	}
}
