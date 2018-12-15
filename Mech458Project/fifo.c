/*
 * fifo.c
 *
 * Created: 2018-11-29 9:57:29 PM
 *  Author: ntron
 */ 

#include "fifo.h"

static void FIFO_Decr(Fifo* fifo);

void FIFO_Init(Fifo* fifo)
{
	fifo->len = 0;
	fifo->back = FIFO_CAPACITY - 1;
	
	for(uint16_t i = 0; i < FIFO_CAPACITY; i++)
	{
		fifo->buffer[i].counter = 0;
		fifo->buffer[i].isFerroMag = false;
		fifo->buffer[i].reflectivity = LARGEST_UINT16_T;
		fifo->buffer[i].class = UNCLASSIFIED;
	}
}

void FIFO_Reset(Fifo* fifo)
{
	fifo->len = 0;
	fifo->back = FIFO_CAPACITY - 1;
}

static void FIFO_Decr(Fifo* fifo)
{
	if (fifo->back == 0)
		fifo->back = FIFO_CAPACITY - 1;
	else
		fifo->back--;
}

void FIFO_Push(Fifo* fifo, FifoElem val)
{
	if (fifo->len != 0)
		FIFO_Decr(fifo);
	
	fifo->buffer[fifo->back] = val;
	fifo->len++;
}

bool FIFO_IsEmpty(Fifo* fifo)
{
	if(fifo->len == 0)
		return true;
	else
		return false;
}

FifoElem FIFO_Pop(Fifo* fifo)
{
	FifoElem ret = DEFAULT_FIFO_ELEM;
	
	if(fifo->len != 0)
	{
		ret = fifo->buffer[(fifo->back + fifo->len - 1) % FIFO_CAPACITY];
		fifo->len--;
			
		// if the fifo is now empty after the pop, then use the opportunity to reset
		if (fifo->len == 0)
			fifo->back = FIFO_CAPACITY - 1;
	}
	
	return ret;
}

FifoElem FIFO_Peak(Fifo* fifo)
{
	FifoElem ret = DEFAULT_FIFO_ELEM;
	
	if (fifo->len != 0)
		ret = fifo->buffer[(fifo->back + fifo->len - 1) % FIFO_CAPACITY];
	
	return ret;
}

FifoElem FIFO_PeakSecond(Fifo* fifo)
{
	FifoElem ret = DEFAULT_FIFO_ELEM;
	
	if (fifo->len != 0)
		ret = fifo->buffer[(fifo->back + fifo->len - 2) % FIFO_CAPACITY];
	
	return ret;
}

uint16_t FIFO_Size(Fifo* fifo)
{
	return fifo->len;
}

volatile FifoElem* FIFO_BackPtr(Fifo* fifo)
{
	return &(fifo->buffer[fifo->back]);
}
