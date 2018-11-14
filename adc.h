#ifndef MY_ADC_H
#define MY_ADC_H

#include <avr/io.h>
#include <avr/interrupt.h>
#include "config.h"

typedef struct ADC
{
	uint16_t result;
	bool result_finished;
} ADC;

void ADC_Init();
void ADC_StartConversion(ADC* adc);
void ADC_ReadConversion(ADC* adc);
#endif // MY_ADC_H