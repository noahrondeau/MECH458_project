/*
 * Filter.c
 *
 * Created: 2018-11-23 3:28:45 PM
 *  Author: ntron
 */ 

#include "Filter.h"
#include "config.h"

// static variables representing last 2 inputs and outputs
static volatile accum xn_1;
static volatile accum xn_2;
static volatile accum yn_1;
static volatile accum yn_2;

// init or reset: pad the input and outputs with desired value.
// this minimizes the transient at startup
// if the padding value is close to the initial value
void FILTER_InitReset(accum padVal)
{	
	xn_1 = padVal;
	yn_1 = padVal;
	xn_2 = padVal;
	yn_2 = padVal;
}

// implements the IIR difference equation
accum Filter(accum xn)
{
	// calculate the output
	accum yn = (IIRA1*yn_1) + (IIRA2*yn_2) + (IIRB0*xn) + (IIRB1*xn_1) + (IIRB2*xn_2);
	
	// store the new input and output, get rid of oldest
	xn_2 = xn_1;
	xn_1 = xn;
	yn_2 = yn_1;
	yn_1 = yn;
	
	// return output
	return yn;
}