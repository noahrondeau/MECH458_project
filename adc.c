
#include "ADC.h"
#include "timer.h"

void ADC_Init(ADC* adc)
{
	// config ADC
	ADCSRA |= _BV(ADEN); // enable ADC
	ADCSRA |= _BV(ADIE); // enable interrupt of ADC

	//sets ADLAR 0 which makes ADCL 8bits read 1st ADCH 2 bits read 2nd
	/* ADLAR = 0 sets the adc registers to the following:
	ADCH = |0|0|0|0|0|0|ADC9|ADC8|
	ADCL = |ADC7|ADC6|ADC5|ADC4|ADC3|ADC2|ADC1|ADC0| */
	ADMUX |= _BV(REFS0); //sets to use refrence voltage with capacitor
	//sets adc to channel 1: reflective sensor, Input PORTF1 
	ADMUX |= _BV(MUX0); 
	// initialize the ADC, start one conversion at the beginning ==========

	adc->result_finished = false;
}

void ADC_StartConversion(ADC* adc)
{
	ADCSRA |= _BV(ADSC);
}
void ADC_ReadConversion(ADC* adc)
{
	uint8_t ADC_result_low = ADCL;
	uint8_t ADC_result_high = ADCH;
	adc->result = (uint16_t)( ((uint16_t)ADC_result_low) | (((uint16_t)ADC_result_high) << 8)  );
}

ISR(ADC_vect)
{
}