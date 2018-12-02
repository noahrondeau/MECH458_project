/*
 * Timer.h
 *
 * Created: 2018-11-13 11:36:00 PM
 *  Author: ntron
 */ 


#ifndef TIMER_H_
#define TIMER_H_


void TIMER1_DelayInit(void);
void TIMER1_DelayUs(uint16_t ms);

void TIMER2_DelayInit(void);
void TIMER2_DelayMs(uint16_t us);

void TIMER3_InterruptInit(void);


#endif /* TIMER_H_ */