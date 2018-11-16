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

typedef volatile struct ADCHandle
{
	uint16_t result;
	bool result_finished;
} ADCHandle;


void ADC_Init();
void ADC_StartConversion(ADCHandle* adc);
void ADC_ReadConversion(ADCHandle* adc);



#endif /* ADC_H_ */