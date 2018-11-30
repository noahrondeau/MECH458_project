/*
 * uart.c
 *
 * Created: 2018-11-29 10:00:43 PM
 *  Author: ntron
 */ 

#include "uart.h"
#include "LedBank.h"

void uint16ToString(uint16_t byte, char* out)
{
	// store the character in the inverted order, since we will be sending them as strings
	for(int i = 0; i < 16; i++)
	out[15-i] = (((byte >> i) & 0x01) == 1) ? '1' : '0';
}

void UART_Init(uint32_t baudrate)
{
	//empty
}

void UART_SendChar(char c)
{
	// empty
	LED_Set((uint8_t)c);
}
void UART_SendString(const char* s, uint16_t len)
{
	for( int i = 0; i < len; i++)
	UART_SendChar(s[i]);
}