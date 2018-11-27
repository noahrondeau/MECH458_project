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

void TIMER1_DelayInit(void)
{
	//prescale to 1/8 to tick at a 1MHz rate
	TCCR1B |= _BV(CS11);
}

void TIMER1_DelayMs(int ms)
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

void TIMER2_DelayInit(void)
{
	TCCR2B |= _BV(CS21);
}

void TIMER2_DelayUs(int us){
	// Index for loop
	int i = 0;
	// Set Waveform Gen Mode to CTC mode
	TCCR2A |= _BV(WGM21);
	// Set output compare register to count 1cycles = 1us
	OCR2A = 0x0001;
	// Reset the timer
	TCNT2 = 0x0000;

	// clear interrupt flag and begin counting
	TIFR2 |= _BV(OCF2A);

	// loop "ms" times, each loop iteration is one millisecond
	while (i < us)
	{
		// wait for the timer to be reached
		while ((TIFR2 & TIMER_COUNT_REACHED) != TIMER_COUNT_REACHED);
		// increment loop counter
		i++;
		// clear interrupt flag and begin counting
		TIFR2 |= _BV(OCF2A);
	}
}

void TIMER3_DelayInit(void)
{
	//prescale to 1/8 to tick at a 1MHz rate
	TCCR3B |= _BV(CS31);

	
}

void TIMER3_DelayMs(int ms)
{
	// Index for loop
	int i = 0;
	// Set Waveform Gen Mode to CTC mode
	TCCR3B |= _BV(WGM32);
	// Set output compare register to count 1000 cycles = 1ms
	OCR3A = 0x03e8;
	// Reset the timer
	TCNT3 = 0x0000;

	// clear interrupt flag and begin counting
	TIFR3 |= _BV(OCF3A);

	// loop "ms" times, each loop iteration is one millisecond
	while (i < ms)
	{
		// wait for the timer to be reached
		while ((TIFR3 & TIMER_COUNT_REACHED) != TIMER_COUNT_REACHED);
		// increment loop counter
		i++;
		// clear interrupt flag and begin counting
		TIFR3 |= _BV(OCF3A);
	}
}

void TIMER3_InterruptInit()
{
	TCCR3B |= _BV(CS32); //set prescale to 1/256
	TCCR3B |= _BV(WGM32); //CTC mode
	TIMSK3 |= _BV(OCIE3B); //set enable interrupt on output compare B
	OCR3B = 0x7A12;  //set to 1s compare - 31250 clock ticks
	TCNT3 = 0x0000;
	TIFR3 |= _BV(OCF3B); //Clear interrupt flag begin counting
	
}