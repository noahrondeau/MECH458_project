/*
 * Filter.c
 *
 * Created: 2018-11-23 3:28:45 PM
 *  Author: ntron
 */ 

#include "Filter.h"
#include "config.h"

static volatile struct
{
	// circular buffer inputs and outputs
	uint8_t currInputIndex;
	uint8_t currOutputIndex;
	accum input[FILTER_NUMER_LEN];
	accum output[FILTER_DENOM_LEN];
} __myStaticIIR;

// call this to reset the input and output buffers to a desired value
// ideally, this value should be set as close to the expect first input in the data stream
// as possible, to minimize the startup transient
void FILTER_ResetWithPadding(accum padVal)
{
	for ( uint8_t i = 0; i < FILTER_NUMER_LEN; i++ )
		__myStaticIIR.input[i] = padVal;
	for ( uint8_t i = 0; i < FILTER_DENOM_LEN; i++ )
		__myStaticIIR.output[i] = padVal;

	__myStaticIIR.currInputIndex = FILTER_NUMER_LEN - 1;
	__myStaticIIR.currOutputIndex = FILTER_NUMER_LEN - 1;
}

void FILTER_Init(accum padVal)
{	
	FILTER_ResetWithPadding(padVal);
}

void PushFilterInput(accum val)
{
	//Push onto circular input buffer, this is an O(1) operation
	if (__myStaticIIR.currInputIndex == 0 ) __myStaticIIR.currInputIndex = FILTER_NUMER_LEN - 1;
	else __myStaticIIR.currInputIndex--;
	__myStaticIIR.input[__myStaticIIR.currInputIndex] = val;
}

void PushFilterOutput(accum val)
{
	// push onto circular output buffer, this is an O(1) operation	
	if (__myStaticIIR.currOutputIndex == 0) __myStaticIIR.currOutputIndex = FILTER_DENOM_LEN - 1;
	else __myStaticIIR.currOutputIndex--;
	__myStaticIIR.output[__myStaticIIR.currOutputIndex] = val;
}

accum GetInput(uint8_t index)
{
	return __myStaticIIR.input[(__myStaticIIR.currInputIndex + index) % FILTER_NUMER_LEN];
}

accum GetOutput(uint8_t index)
{
	return __myStaticIIR.output[(__myStaticIIR.currOutputIndex + index) % FILTER_DENOM_LEN];
}

accum Filter(uint16_t next)
{
	accum yn = 0.0K; // will hold return value
	// push a new input in, get rid of un-needed past input
	PushFilterInput((accum)next);
	
	// implements the IIR different equation
	// y[n] =   a_1*y[n-1] + ... + a_k*y[n-k]
	//		  + b_0*x[n] + b_1*x[n-1] + ... + b_j*x[n-j]
	yn += (IIRB0*GetInput(0));
	yn += (IIRB1*GetInput(1));
	yn += (IIRB2*GetInput(2));
	yn += (IIRB3*GetInput(3));
	
	yn += (IIRA0*GetOutput(0));
	yn += (IIRA1*GetOutput(1));
	yn += (IIRA2*GetOutput(2));
	
	// stash new output for future use in equation
	PushFilterOutput(yn);
	
	return yn;
}