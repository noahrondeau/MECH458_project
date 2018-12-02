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
static volatile accum last_input;
static volatile accum last_output;

// public functions
void FILTER_InitReset(accum padVal)
{	
	last_input = padVal;
	last_output = padVal;
}

accum Filter(accum new_input)
{
	accum new_output = (IIRA1*last_output) + (IIRB0*new_input) + (IIRB1*last_input);
	last_input = new_input;
	last_output = new_output;
	return new_output;
	
}