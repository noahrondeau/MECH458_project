/*
 * Timer.c
 *
 * Created: 2018-11-13 11:49:43 PM
 *  Author: ntron
 */ 



#include <avr/io.h>
#include <avr/interrupt.h>
#include "Timer.h"
#include "config.h"

#define TIMER_COUNT_REACHED (0x02)
#define LED_DIR_LEFT (0)
#define LED_DIR_RIGHT (1)
#define LED_RIGHT_END (0b00000011)
#define LED_LEFT_END (0b11000000)

/* ====== TIMER1 functions ======*/
// TIMER 1 is used for millisecond timing
// it can be used either for delay or for interrupt-driven operation, but not simultaneously

// initlializes timer 1
void TIMER1_Init(void)
{
	//prescale to 1/8 to tick at a 1MHz rate
	TCCR1B |= _BV(CS11);
	// Set Waveform Gen Mode to CTC mode
	TCCR1B |= _BV(WGM12);
}

// delay for a certain number of milliseconds
void TIMER1_DelayUs(uint16_t us)
{
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		// turn off interrupts if they are on:
		TIMSK1 &= ~(1<<OCIE1A);
		// Set output compare register to count "us" cycles
		OCR1A = us;
		// Reset the timer
		TCNT1 = 0x0000;
		// clear interrupt flag and begin counting
		TIFR1 |= _BV(OCF1A);
	}
	
	// wait for "us" ticks to be reached
	while ((TIFR1 & TIMER_COUNT_REACHED) != TIMER_COUNT_REACHED);
}

void TIMER1_EnableInt(void)
{
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		// turn on interrupts
		TIMSK1 |= (1<<OCIE1A);
	}
}
void TIMER1_DisableInt(void)
{
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		// turn off interrupts
		TIMSK1 &= ~(1<<OCIE1A);	
	}
}

// interrupt on output match A after a given number of microseconds
void TIMER1_ScheduleIntUs(uint16_t us)
{
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		// Set output compare register to count "us" cycles
		OCR1A = us;
		// Reset the timer
		TCNT1 = 0x0000;
	}
}

/* ====== TIMER2 Functions ====== */
// timer 2 is used exclusively for the millisecond delays

void TIMER2_DelayInit(void)
{
	TCCR2B |= _BV(CS21); // 1MH clock rate
}

void TIMER2_DelayMs(uint16_t ms){
	
	// this is an 8-bit timer, the max output compare is 255
	// we need to keep setting it to 100, and then loop 10*ms
	// Index for loop
	uint16_t i = 0;
	
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		// Set Waveform Gen Mode to CTC mode
		TCCR2A |= _BV(WGM21);
		// Set output compare register to count 100
		OCR2A = 0x64;
		// Reset the timer
		TCNT2 = 0x0000;

		// clear interrupt flag and begin counting
		TIFR2 |= _BV(OCF2A);
	}

	// loop "10*ms" times, each loop iteration is one millisecond
	while (i < 10*ms)
	{
		// wait for the timer to be reached
		while ((TIFR2 & TIMER_COUNT_REACHED) != TIMER_COUNT_REACHED);
		// increment loop counter
		i++;
		// clear interrupt flag and begin counting
		TIFR2 |= _BV(OCF2A);
	}
}

void TIMER3_InterruptInit(void)
{

	//Scale by 1024, set WGM32 = 1 for CTC comp at OCR3A
	TCCR3B |= _BV(CS32) | _BV(CS30) | _BV(WGM32);
	//Enable Output compare comp A interrupt
	TIMSK3 |= _BV(OCIE3A);
	//clear counter
	TCNT3 = 0x0000;
	//Set compare to 62500 -> timer for 8s
	OCR3A = 0xF424;
	//Start counting
	TIFR3 |= _BV(OCF3A);
	
}

