/*
 * IncFile1.h
 *
 * Created: 2018-11-23 3:13:24 PM
 *  Author: ntron
 */ 



#ifndef INCFILE1_H_
#define INCFILE1_H_

#include "config.h"

void FILTER_InitReset(accum padVal);

// Utility functions
void PushFilterOutput(accum val);
void PushFilterInput(accum val);

accum GetInput(uint8_t index);
accum GetOutput(uint8_t index);


// WARNING! need to avoid overflow, so arithmetic must be done carefully!

/*	@brief: an IIR filter implementation
 *	@return: the latest filter output 
 *
 *	This filter does not guarantee safety against overflow, either in the calculations or in the output
 *  It will perform calculations in the safest manner possible, casting to larger types for calcs,
 *	but beware.
 *	Use small coefficients if you know what is good for you
 *	NOTE: the output could end up negative at certain points for values very close to 0 in a transient
 *			so noise could possibly make the output negative. Check for negative accums before casting to an unsigned type!
 */
accum Filter(uint16_t next);


#endif /* INCFILE1_H_ */