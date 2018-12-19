/*
 * IncFile1.h
 *
 * Created: 2018-11-23 3:13:24 PM
 *  Author: ntron
 */ 

/* this module implements a 2nd order IIR butterworth filter.
 * it uses the AVR-GCC native fixed point library for efficiency and accuracy
 * instead of inefficient floating point calculations.
 * Coefficients for the filter are found in config.h
 */

#ifndef FILTER_H
#define FILTER_H

#include "config.h"

void FILTER_InitReset(accum padVal);
accum Filter(accum xn);


#endif /* INCFILE1_H_ */