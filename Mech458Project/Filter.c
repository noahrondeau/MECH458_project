/*
 * Filter.c
 *
 * Created: 2018-11-23 3:28:45 PM
 *  Author: ntron
 */ 

#include "Filter.h"
#include "config.h"

// static variables
// circular buffer inputs and outputs
static volatile	uint8_t currInputIndex;
static volatile uint8_t currOutputIndex;
static volatile accum input[FILTER_NUMER_LEN];
static volatile accum output[FILTER_DENOM_LEN];

// Utility functions forward declare
static void PushFilterOutput(accum val);
static void PushFilterInput(accum val);
static accum GetInput(uint8_t index);
static accum GetOutput(uint8_t index);

// public functions
void FILTER_InitReset(accum padVal)
{	
	for ( uint8_t i = 0; i < FILTER_NUMER_LEN; i++ )
		input[i] = padVal;
	for ( uint8_t i = 0; i < FILTER_DENOM_LEN; i++ )
		output[i] = padVal;

	currInputIndex = FILTER_NUMER_LEN - 1;
	currOutputIndex = FILTER_NUMER_LEN - 1;
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


/* ====== Utility function defs ====== */
static void PushFilterInput(accum val)
{
	//Push onto circular input buffer, this is an O(1) operation
	if (currInputIndex == 0 )
		currInputIndex = FILTER_NUMER_LEN - 1;
	else
		currInputIndex--;
		
	input[currInputIndex] = val;
}

static void PushFilterOutput(accum val)
{
	// push onto circular output buffer, this is an O(1) operation	
	if (currOutputIndex == 0)
		currOutputIndex = FILTER_DENOM_LEN - 1;
	else currOutputIndex--;
		output[currOutputIndex] = val;
}

static accum GetInput(uint8_t index)
{
	return input[(currInputIndex + index) % FILTER_NUMER_LEN];
}

static accum GetOutput(uint8_t index)
{
	return output[(currOutputIndex + index) % FILTER_DENOM_LEN];
}