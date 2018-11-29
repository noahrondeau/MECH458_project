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
accum Filter(uint16_t next);


#endif /* INCFILE1_H_ */