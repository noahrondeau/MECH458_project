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

// Utility functions forward declare
static volatile accum xn_1;
static volatile accum xn_2;
static volatile accum yn_1;
static volatile accum yn_2;

// public functions
void FILTER_InitReset(accum padVal)
{	
	xn_1 = padVal;
	yn_1 = padVal;
	xn_2 = padVal;
	yn_2 = padVal;
}

accum Filter(accum xn)
{
	accum yn = (IIRA1*yn_1) + (IIRA2*yn_2) + (IIRB0*xn) + (IIRB1*xn_1) + (IIRB2*xn_2);
	xn_2 = xn_1;
	xn_1 = xn;
	yn_2 = yn_1;
	yn_1 = yn;
	return yn;
	
}