/*
 * ADC.h
 *
 * Created: 2018-11-13 11:23:02 PM
 *  Author: ntron
 */ 


#ifndef ADC_H_
#define ADC_H_

#include <avr/io.h>
#include <avr/interrupt.h>
#include "config.h"

typedef enum ADCPrescale
{
	// PRESCALER TABLE:
	//			ADCSRA[7:3] | ADPS[2] | ADPS[1] | ADPS[0]	||	Division factor	||	ADC_CLK speed (8MHz sysclk)
	//				x			0			0		0		||		2			||		4 MHz
	//				x			0			0		1		||		2			||		4 MHz
	//				x			0			1		0		||		4			||		2 MHz
	//				x			0			1		1		||		8			||		1 MHz
	//				x			1			0		0		||		16			||		500 KHz
	//				x			1			0		1		||		32			||		250 KHz
	//				x			1			1		0		||		64			||		125 KHz
	//				x			1			1		1		||		128			||		62.5 KHz
	ADC_PRESCALE_2		=	0b00000001,
	ADC_PRESCALE_4		=	0b00000010,
	ADC_PRESCALE_8		=	0b00000011,
	ADC_PRESCALE_16		=	0b00000100,
	ADC_PRESCALE_32		=	0b00000101,
	ADC_PRESCALE_64		=	0b00000110,
	ADC_PRESCALE_128	=	0b00000111,
} ADCPrescale;

typedef volatile struct ADCHandle
{
	uint16_t result;
	bool result_finished;
} ADCHandle;


void ADC_Init(ADCHandle* adc, ADCPrescale prescale);
void ADC_StartConversion(ADCHandle* adc);
void ADC_ReadConversion(ADCHandle* adc);



#endif /* ADC_H_ */