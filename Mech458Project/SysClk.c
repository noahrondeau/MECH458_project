/*
 * SysClk.c
 *
 * Created: 2018-11-18 10:30:34 AM
 *  Author: Matthew
 */ 

#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdlib.h>
#include "SysClk.h"

void Sys_Clk_Init(/*unsigned char prescaler*/){

	// set CLKPR bit to change clock prescaler
	CLKPR = (1<<CLKPCE);
	// no clock prescaler
	CLKPR = 0;
}
