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
void FILTER_Reset(DigitalFilter* f, uint16_t padVal)
{
	for ( uint8_t i = 0; i < FILTER_NUMER_LEN; i++ )
		f->input[i] = padVal;
	for ( uint8_t i = 0; i < FILTER_DENOM_LEN; i++ )
		f->output[i] = padVal;

	f->currInputIndex = FILTER_NUMER_LEN - 1;
	f->currOutputIndex = FILTER_NUMER_LEN - 1;
}

void PushFilterInput(DigitalFilter* f, uint16_t val)
{
	//Push onto circular input buffer, this is an O(1) operation
	if (f->currInputIndex == 0 ) f->currInputIndex = FILTER_NUMER_LEN - 1;
	else f->currInputIndex--;
	f->input[f->currInputIndex] = val;
}

void PushFilterOutput(DigitalFilter* f, uint16_t val)
{
	// push onto circular output buffer, this is an O(1) operation	
	if (f->currOutputIndex == 0) f->currOutputIndex = FILTER_DENOM_LEN - 1;
	else f->currOutputIndex--;
	f->output[f->currOutputIndex] = val;
}

uint16_t GetInput(DigitalFilter* f, uint8_t index)
{
	return f->input[(f->currInputIndex + index) % FILTER_NUMER_LEN];
}

uint16_t GetOutput(DigitalFilter* f, uint8_t index)
{
	return f->output[(f->currOutputIndex + index) % FILTER_DENOM_LEN];
}

uint16_t Filter(DigitalFilter* f, uint16_t new_input)
{
	// this calculation is done in signed integers, but calculating the remainder
	// and aggregating to lose as little precision to truncation as possible
	int16_t x_quo_sum = 0; // will hold the sum of quotients of the feedforward terms
	int16_t x_mod_sum = 0; // will hold the sum of remainders of the feedforward terms
	int16_t y_quo_sum = 0; // will hold the sum of quotients of the feedback terms
	int16_t y_mod_sum = 0; // will hold the sum of remainders of the feedback terms
	int16_t x_sum;
	int16_t y_sum;
	int16_t sum;
	
	
	// push a new input in, get rid of un-needed past input
	PushFilterInput(f, new_input);
	
	// implements the IIR different equation
	// y[n] =   a_1*y[n-1] + ... + a_k*y[n-k]
	//		  + b_0*x[n] + b_1*x[n-1] + ... + b_j*x[n-j]
	
	// we do this in signed integers, but acummulate the modulo to truncate as little as possible
	
	// add all numerator (input terms)
	x_quo_sum += FILTER_B0 * (int16_t)GetInput(f,0) / FILTER_B_COMMON_DENOM;
	x_quo_sum += FILTER_B1 * (int16_t)GetInput(f,1) / FILTER_B_COMMON_DENOM;
	x_quo_sum += FILTER_B2 * (int16_t)GetInput(f,2) / FILTER_B_COMMON_DENOM;
	x_quo_sum += FILTER_B3 * (int16_t)GetInput(f,3) / FILTER_B_COMMON_DENOM;
	
	x_mod_sum += FILTER_B0 * (int16_t)GetInput(f,0) % FILTER_B_COMMON_DENOM;
	x_mod_sum += FILTER_B1 * (int16_t)GetInput(f,1) % FILTER_B_COMMON_DENOM;
	x_mod_sum += FILTER_B2 * (int16_t)GetInput(f,2) % FILTER_B_COMMON_DENOM;
	x_mod_sum += FILTER_B3 * (int16_t)GetInput(f,3) % FILTER_B_COMMON_DENOM;
	
	x_sum = x_quo_sum + (x_mod_sum / FILTER_B_COMMON_DENOM);
	
	
	// add all denominator (output) terms
	y_quo_sum += FILTER_A1 * (int16_t)GetOutput(f,0) / FILTER_A_COMMON_DENOM;
	y_quo_sum += FILTER_A2 * (int16_t)GetOutput(f,1) / FILTER_A_COMMON_DENOM;
	y_quo_sum += FILTER_A3 * (int16_t)GetOutput(f,2) / FILTER_A_COMMON_DENOM;
	
	y_mod_sum += FILTER_A1 * (int16_t)GetOutput(f,0) % FILTER_A_COMMON_DENOM;
	y_mod_sum += FILTER_A2 * (int16_t)GetOutput(f,1) % FILTER_A_COMMON_DENOM;
	y_mod_sum += FILTER_A3 * (int16_t)GetOutput(f,2) % FILTER_A_COMMON_DENOM;
	
	y_sum = y_quo_sum + (y_mod_sum / FILTER_A_COMMON_DENOM);
	
	sum = y_sum + x_sum;
	if ( sum < 0 )
		sum = 0;
	else if (sum > 1023)
		sum = 1023;
	// stash new output for future use in equation
	PushFilterOutput(f,(uint16_t)sum);
	
	// return value as integer
	return (uint16_t)sum;
}