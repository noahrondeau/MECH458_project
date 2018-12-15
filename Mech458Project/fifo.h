/*
 * fifo.h
 *
 * Created: 2018-11-29 9:57:17 PM
 *  Author: ntron
 */ 


#ifndef FIFO_H_
#define FIFO_H_

#include "config.h"

#define FIFO_CAPACITY (100)

typedef struct FifoElem
{
	unsigned int counter;
	bool isFerroMag;
	uint16_t reflectivity;
	uint16_t sampleCount;
	ItemClass class;
} FifoElem;

typedef volatile struct Fifo
{
	FifoElem buffer[FIFO_CAPACITY];
	uint16_t back;
	uint16_t len;
} Fifo;

#define DEFAULT_FIFO_ELEM	{.counter = 0, .isFerroMag = false, .reflectivity = LARGEST_UINT16_T, .class = UNCLASSIFIED}


void FIFO_Init(Fifo* fifo);
void FIFO_Reset(Fifo* fifo);
void FIFO_Push(Fifo* fifo, FifoElem val);
FifoElem FIFO_Pop(Fifo* fifo);
FifoElem FIFO_Peak(Fifo* fifo);
FifoElem FIFO_PeakSecond(Fifo* fifo);
bool FIFO_IsEmpty(Fifo* fifo);
uint16_t FIFO_Size(Fifo* fifo);
volatile FifoElem* FIFO_BackPtr(Fifo* fifo);

#endif /* FIFO_H_ */