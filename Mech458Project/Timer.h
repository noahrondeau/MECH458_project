/*
 * Timer.h
 *
 * Created: 2018-11-13 11:36:00 PM
 *  Author: ntron
 */ 


#ifndef TIMER_H_
#define TIMER_H_


void TIMER1_DelayInit(void);
void TIMER1_DelayMs(int ms);

void TIMER_ScheduleInit(void);
void TIMER_ScheduleMs(int ms);

void TIMER3_DelayInit(void);
void TIMER3_DelayMs(int ms);

void TIMER3_InterruptInit(void);


#endif /* TIMER_H_ */