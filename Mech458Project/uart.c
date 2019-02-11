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

void UART_Init()
{
	// set baud to 9600 -> UBRRn = (sysclock_f / 16*baud) - 1 = 51 = 0b00110011
	//empty
	UBRR1 = (uint8_t)51;
	// Enable transmitter
	UCSR1B = (1<<TXEN1);
	// Set frame format: 8data, 1stop bit, no parity (8N1)
	UCSR1C = (1<<UCSZ10)| (1 << UCSZ11);
}

// send a single ascii character
void UART_SendChar(char c)
{
	while ( !( UCSR1A & (1<<UDRE1)) );
	UDR1 = c;
}

// send a null-char terminated string over ascii
void UART_SendString(const char* s)
{
	unsigned int counter = 0;
	while( s[counter] != '\0')
	{
		UART_SendChar(s[counter]);
		counter++;
	}
}