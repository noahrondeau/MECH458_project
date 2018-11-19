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

typedef volatile struct LedBank
{
	bool ledStatus[LED_BANK_LEN];
	uint8_t ledMask[LED_BANK_LEN];
} LedBank;

void LED_Init(LedBank* ledBank);
void LED_on(LedBank* ledBank, uint8_t led);
void LED_off(LedBank* ledBank, uint8_t led);
void LED_toggle(LedBank* ledBank, uint8_t led);
void LED_set(LedBank* ledBank, uint8_t seq);



#endif /* LEDBANK_H_ */