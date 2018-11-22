/*
 * LedBank.h
 *
 * Created: 2018-11-19 1:51:23 PM
 *  Author: ntron
 */ 

#ifndef LEDBANK_H_
#define LEDBANK_H_

#include "config.h"
#include <avr/io.h>

#define LED_BANK_LEN (8)

void LED_Init();
void LED_On(uint8_t led);
void LED_Off(uint8_t led);
void LED_Toggle(uint8_t led);
void LED_Set(uint16_t seq);
void LED_SetBottom8(uint8_t seq);



#endif /* LEDBANK_H_ */