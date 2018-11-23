/*
 * ADC.c
 *
 * Created: 2018-11-13 11:40:48 PM
 *  Author: ntron
 */ 


#include "ADC.h"
#include "Timer.h"

void ADC_Init(ADCHandle* adc)
{
	// config ADC
	ADCSRA |= _BV(ADEN); // enable ADC
	
	// this should probably go at the end so no interrupt gets queued for the startup conversion
	ADCSRA |= _BV(ADIE); // enable interrupt of ADC

	//sets ADLAR 0 which makes ADCL 8bits read 1st ADCH 2 bits read 2nd
	/* ADLAR = 0 sets the adc registers to the following:
	ADCH = |0|0|0|0|0|0|ADC9|ADC8|
	ADCL = |ADC7|ADC6|ADC5|ADC4|ADC3|ADC2|ADC1|ADC0| */
	ADMUX |= _BV(REFS0); //sets to use refrence voltage with capacitor
	//sets adc to channel 1: reflective sensor, Input PORTF1 
	ADMUX |= _BV(MUX0); 
	// initialize the ADC, start one conversion at the beginning
	ADC_StartConversion(adc);
	// wait because no interupts are enabled yet -- first conversion takes 25 clock cycles
	TIMER1_DelayMs(1);
	// poll for conversion value
	ADC_ReadConversion(adc);
	// clear useless conversion result
	adc->result = 0;
	adc->result_finished = false;
}

void ADC_StartConversion(ADCHandle* adc)
{
	ADCSRA |= _BV(ADSC);
}


void ADC_ReadConversion(ADCHandle* adc)
{
	uint8_t ADC_result_low = ADCL;
	uint8_t ADC_result_high = ADCH;
	adc->result = (uint16_t)( ((uint16_t)ADC_result_low) | (((uint16_t)ADC_result_high) << 8)  );
}