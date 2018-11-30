/*
 * fifo.h
 *
 * Created: 2018-11-29 9:57:17 PM
 *  Author: ntron
 */ 


#ifndef FIFO_H_
#define FIFO_H_

#include "config.h"

#define FIFO_CAPACITY (50)
#define DEFAULT_FIFOELEM (0)

volatile struct {
	uint16_t buffer[FIFO_CAPACITY];
	uint16_t back;
	uint16_t len;
} fifo;

void FIFO_Init();
void FIFO_Reset();
void FIFO_Push();
uint16_t FIFO_Pop();
uint16_t FIFO_Peak();
void FIFO_Decr();
bool FIFO_IsEmpty();

#endif /* FIFO_H_ */