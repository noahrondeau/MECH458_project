/*
 * IncFile1.h
 *
 * Created: 2018-11-23 3:13:24 PM
 *  Author: ntron
 */ 



#ifndef FILTER_H
#define FILTER_H

#include "config.h"

void FILTER_InitReset(accum padVal);
accum Filter(accum new_input);


#endif /* INCFILE1_H_ */