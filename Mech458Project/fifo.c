/*
 * fifo.c
 *
 * Created: 2018-11-29 9:57:29 PM
 *  Author: ntron
 */ 

#include "fifo.h"


void FIFO_Init()
{
	fifo.len = 0;
	fifo.back = FIFO_CAPACITY - 1;
	
	for(uint16_t i = 0; i < FIFO_CAPACITY; i++)
	fifo.buffer[i] = DEFAULT_FIFOELEM;
}

void FIFO_Reset()
{
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		fifo.len = 0;
		fifo.back = FIFO_CAPACITY - 1;
	}
}

void FIFO_Decr()
{
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		if (fifo.back == 0)
			fifo.back = FIFO_CAPACITY - 1;
		else
			fifo.back--;
	}
}

void FIFO_Push(uint16_t val)
{
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		if (fifo.len != 0)
			FIFO_Decr();
		
		fifo.buffer[fifo.back] = val;
		fifo.len++;
	}
}

bool FIFO_IsEmpty()
{
	bool ret;
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		if(fifo.len == 0)
			ret = true;
		else
			ret = false;
	}
	return ret;
}

uint16_t FIFO_Pop()
{
	uint16_t ret = DEFAULT_FIFOELEM;
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		if(fifo.len != 0)
		{
			ret = fifo.buffer[(fifo.back + fifo.len - 1) % FIFO_CAPACITY];
			fifo.len--;
			
			// if the fifo is now empty after the pop, then use the opportunity to reset
			if (fifo.len == 0)
			fifo.back = FIFO_CAPACITY - 1;
		}
	}
	return ret;
}

uint16_t FIFO_Peak()
{
	uint16_t ret = DEFAULT_FIFOELEM;
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		if (fifo.len != 0)
		ret = fifo.buffer[(fifo.back + fifo.len - 1) % FIFO_CAPACITY];
	}
	return ret;
}