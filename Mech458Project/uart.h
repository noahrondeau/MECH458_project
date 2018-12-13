/*
 * uart.h
 *
 * Created: 2018-11-29 9:43:25 PM
 *  Author: ntron
 */ 


#ifndef UART_H_
#define UART_H_

#include "config.h"

void uint16ToString(uint16_t byte, char* out);
void UART_Init();
void UART_SendChar(char c);
void UART_SendString( const char* s);


#endif /* UART_H_ */