/*
 * IncFile1.h
 *
 * Created: 2018-11-23 3:13:24 PM
 *  Author: ntron
 */ 



#ifndef INCFILE1_H_
#define INCFILE1_H_

#include "config.h"

typedef volatile struct DigitalFilter
{
	// circular buffer inputs and outputs
	uint8_t currInputIndex;
	uint8_t currOutputIndex;
	uint16_t input[FILTER_NUMER_LEN];
	uint16_t output[FILTER_DENOM_LEN];
} DigitalFilter;

void FILTER_Reset(DigitalFilter* f, uint16_t padVal);

// Utility functions
void PushFilterOutput(DigitalFilter* f, uint16_t val);
void PushFilterInput(DigitalFilter* f, uint16_t val);

uint16_t GetInput(DigitalFilter* f, uint8_t index);
uint16_t GetOutput(DigitalFilter* f, uint8_t index);


// WARNING! need to avoid overflow, so arithmetic must be done carefully!

/*	@brief: an IIR filter implementation
 *	@return: the latest filter output 
 *
 *	This filter does not guarantee safety against overflow, either in the calculations or in the output
 *  It will perform calculations in the safest manner possible, casting to larger types for calcs,
 *	but beware.
 *	Use small coefficients if you know what is good for you
 *	NOTE: the output could end up negative at certain points for values very close to 0 in a transient
 *			so noise could possibly make the output negative. Check for negative floats before casting to an unsigned type!
 */
uint16_t Filter(DigitalFilter* f, uint16_t next);


#endif /* INCFILE1_H_ */