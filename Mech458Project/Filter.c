/*
 * Filter.c
 *
 * Created: 2018-11-23 3:28:45 PM
 *  Author: ntron
 */ 

#include "Filter.h"
#include "config.h"

// call this to reset the input and output buffers to a desired value
// ideally, this value should be set as close to the expect first input in the data stream
// as possible, to minimize the startup transient
void FILTER_ResetWithPadding(DigitalFilter* f, float padVal)
{
	for ( uint8_t i = 0; i < FILTER_NUMER_LEN; i++ )
		f->input[i] = padVal;
	for ( uint8_t i = 0; i < FILTER_DENOM_LEN; i++ )
		f->output[i] = padVal;

	f->currInputIndex = FILTER_NUMER_LEN - 1;
	f->currOutputIndex = FILTER_NUMER_LEN - 1;
}

void FILTER_Init(DigitalFilter* f, float* numerator, float* denominator, float padVal)
{
	for ( uint8_t i = 0; i < FILTER_NUMER_LEN; i++)
	f->num[i] = numerator[i];
	for ( uint8_t i = 0; i < FILTER_DENOM_LEN; i++)
	f->den[i] = denominator[i];
	
	FILTER_ResetWithPadding(f, padVal);
	f->currInputIndex = FILTER_NUMER_LEN - 1;
	f->currOutputIndex = FILTER_DENOM_LEN - 1;
}

void PushFilterInput(DigitalFilter* f, float val)
{
	//Push onto circular input buffer, this is an O(1) operation
	if (f->currInputIndex == 0 ) f->currInputIndex = FILTER_NUMER_LEN - 1;
	else f->currInputIndex--;
	f->input[f->currInputIndex] = val;
}

void PushFilterOutput(DigitalFilter* f, float val)
{
	// push onto circular output buffer, this is an O(1) operation	
	if (f->currOutputIndex == 0) f->currOutputIndex = FILTER_DENOM_LEN - 1;
	else f->currOutputIndex--;
	f->output[f->currOutputIndex] = val;
}

float GetInput(DigitalFilter* f, uint8_t index)
{
	return f->input[(f->currInputIndex + index) % FILTER_NUMER_LEN];
}

float GetOutput(DigitalFilter* f, uint8_t index)
{
	return f->output[(f->currOutputIndex + index) % FILTER_DENOM_LEN];
}

float Filter(DigitalFilter* f, uint16_t new_input)
{
	float yn = 0; // will hold return value
	// push a new input in, get rid of un-needed past input
	PushFilterInput(f,(float)new_input);
	
	// implements the IIR different equation
	// y[n] =   a_1*y[n-1] + ... + a_k*y[n-k]
	//		  + b_0*x[n] + b_1*x[n-1] + ... + b_j*x[n-j]
	for (uint8_t i = 0; i < FILTER_NUMER_LEN; i++)
		yn += (f->num[i])*GetInput(f,i);
		
	for (uint8_t i = 0; i < FILTER_DENOM_LEN; i++)
		yn += (f->den[i])*GetOutput(f,i);
	
	// stash new output for future use in equation
	PushFilterOutput(f,yn);
	
	// return value as integer
	return yn;
}