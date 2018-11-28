/*
 * config.h
 *
 * Created: 2018-11-13 11:19:57 PM
 *  Author: ntron
 */ 


#ifndef CONFIG_H_
#define CONFIG_H_


#include <stdint.h>
#include <stdbool.h>
#include <avr/io.h>
#include <util/atomic.h>
#include <math.h>
#include <stdfix.h>

/* ====== BUILD MODE CONFIG ====== */
#define PROGRAM_MODE	(1)
#define UNITTEST_MODE	(!PROGRAM_MODE)
#define DEBUG_MODE 		(0)

#define LINKED_QUEUE_MODE	(1)
#define CIRCULAR_QUEUE_MODE	(!LINKED_QUEUE_MODE)

#define TWO_PHASE_COMMUTATION_MODE (1)

#define MODE_ENABLED(__mode__)	((__mode__) == 1)
#define MODE_DISABLED(__mode__)	((__mode__) == 0)

#if MODE_ENABLED(DEBUG_MODE)
#include <stdio.h>
#define DEBUG_ONLY(__t__) do{ __t__ ;}while(0)
#else
#define DEBUG_ONLY(__t__)
#endif

/* ====== GPIO PORT REG DEFS ====== */

/*	For ease of maintenance we want to be able to change the pinout of the project
 *	but we don't want to have to change which ports are used everywhere: only in one place.
 *  We initialize which registers are used for which peripheral at run-time init.
 *	To do this, we redefine all the GPIO registers without the deferencing operator, so that
 *	they can be passed around in variables.
 */

typedef volatile uint8_t* GPIOReg;
typedef volatile uint8_t  GPIOMask;

#define PINA_REG		((GPIOReg)((0x00) + (__SFR_OFFSET)))
#define DDRA_REG		((GPIOReg)((0x01) + (__SFR_OFFSET)))
#define PORTA_REG		((GPIOReg)((0x02) + (__SFR_OFFSET)))

#define PINB_REG		((GPIOReg)((0x03) + (__SFR_OFFSET)))
#define DDRB_REG		((GPIOReg)((0x04) + (__SFR_OFFSET)))
#define PORTB_REG		((GPIOReg)((0x05) + (__SFR_OFFSET)))

#define PINC_REG		((GPIOReg)((0x06) + (__SFR_OFFSET)))
#define DDRC_REG		((GPIOReg)((0x07) + (__SFR_OFFSET)))
#define PORTC_REG		((GPIOReg)((0x08) + (__SFR_OFFSET)))

#define PIND_REG		((GPIOReg)((0x09) + (__SFR_OFFSET)))
#define DDRD_REG		((GPIOReg)((0x0A) + (__SFR_OFFSET)))
#define PORTD_REG		((GPIOReg)((0x0B) + (__SFR_OFFSET)))

#define PINE_REG		((GPIOReg)((0x0C) + (__SFR_OFFSET)))
#define DDRE_REG		((GPIOReg)((0x0D) + (__SFR_OFFSET)))
#define PORTE_REG		((GPIOReg)((0x0E) + (__SFR_OFFSET)))

#define PINF_REG		((GPIOReg)((0x0F) + (__SFR_OFFSET)))
#define DDRF_REG		((GPIOReg)((0x10) + (__SFR_OFFSET)))
#define PORTF_REG		((GPIOReg)((0x11) + (__SFR_OFFSET)))

/* ====== STEPPER MOTOR CONFIG ====== */

#define STEPPER_DDR				(DDRA_REG)
#define STEPPER_PORT			(PORTA_REG)

/* ====== DC MOTOR CONFIG ====== */

#define DCMOTOR_PORTX			(PORTB_REG)
#define DCMOTOR_DDRX			(DDRB_REG)

#define DCMOTOR_SPEED			(60)

/* ====== HALL SENSOR CONFIG ====== */

#define HALL_PINX				(PINE_REG)
#define HALL_DDRX				(DDRE_REG)
#define HALL_PORTPIN			(4)

/* ====== OPTICAL SENSOR CONFIG ====== */

#define S1_OPTICAL_PINX			(PIND_REG)
#define S1_OPTICAL_DDRX			(DDRD_REG)
#define S1_OPTICAL_PORTPIN		(1)

#define S2_OPTICAL_PINX			(PIND_REG)
#define S2_OPTICAL_DDRX			(DDRD_REG)
#define S2_OPTICAL_PORTPIN		(2)

#define EXIT_OPTICAL_PINX		(PIND_REG)
#define EXIT_OPTICAL_DDRX		(DDRD_REG)
#define EXIT_OPTICAL_PORTPIN	(0)

/* ====== FERRO SENSOR CONFIG ====== */

#define FERRO_PINX				(PIND_REG)
#define FERRO_DDRX				(DDRD_REG)
#define FERRO_PORTPIN			(3)

/* ====== SW1 PAUSE SWITCH CONFIG ====== */

#define PAUSE_PINX				(PINE_REG)
#define PAUSE_DDRX				(DDRE_REG)
#define PAUSE_PORTPIN			(6)

/* ====== SW2 RAMP-DOWN SWITCH CONFIG ====== */

#define RAMPDOWN_PINX			(PINE_REG)
#define RAMPDOWN_DDRX			(DDRE_REG)
#define RAMPDOWN_PORTPIN		(7)	

/* ====== RL SENSOR CONFIGS ====== */

#define LARGEST_UINT16_T ((uint16_t)(0xFFFF))

#define MIN_ALUMINIUM_VAL		(10.0k)
#define MAX_ALUMINIUM_VAL		(34.0k)
#define RANGE_ALUMINIUM			(MAX_ALUMINIUM_VAL - MIN_ALUMINIUM_VAL)
#define AVG_ALUMINIUM_VAL		(15.8181k)
#define STDEV_ALUMINIUM			(6.6606)

#define MIN_STEEL_VAL			(558.0k)
#define MAX_STEEL_VAL			(664.0k)
#define RANGE_STEEL				(MAX_STEEL_VAL - MIN_STEEL_VAL)
#define AVG_STEEL_VAL			(625.1818k)
#define STDEV_STEEL				(39.7563k)

#define MIN_WHITE_VAL			(893.0k)
#define MAX_WHITE_VAL			(923.0k)
#define RANGE_WHITE				(MAX_WHITE_VAL - MIN_WHITE_VAL)
#define AVG_WHITE_VAL			(908.8181k)
#define STDEV_WHITE				(9.4743k)

#define MIN_BLACK_VAL			(924.0k)
#define MAX_BLACK_VAL			(947.0k)
#define RANGE_BLACK				(MAX_BLACK_VAL - MIN_BLACK_VAL)
#define AVG_BLACK_VAL			(908.8181k)
#define STDEV_BLACK				(7.5546k)

		// CURRENTLY a 3rd order butterworth filter, w_cutoff = 0.1pi rad/sample
#define FILTER_NUMER_LEN		(4)
#define FILTER_DENOM_LEN		(3)
#define FILTER_NUMER_COEFFS		{0.0029, 0.0087, 0.0087, 0.0029}		
#define FILTER_DENOM_COEFFS		{2.3741, -1.9294, 0.5321}

/* ====== GLOBALLY USEFUL TYPEDEFS ====== */


typedef enum ActiveLevel
{
	ACTIVE_LOW = 0x00,
	ACTIVE_HIGH = 0x01,
} ActiveLevel;

typedef enum FsmState
{
	 RUN_STATE,
	 PAUSE_STATE,
	 RAMPDOWN_STATE, 
} FsmState;

typedef volatile struct FiniteStateMachine
{
	FsmState state;
	struct {
		bool beltWasRunning;
		FsmState returnToState;
	} saved;
	bool rampDownInitFlag;
	bool rampDownEndFlag;
} FiniteStateMachine;

typedef enum ItemClass
{
	UNCLASSIFIED = 255, // bogus value, need to check everywhere!!!
	STEEL = 50,
	WHITE_PLASTIC = 100,
	ALUMINIUM = 150,
	BLACK_PLASTIC = 0,
}ItemClass;

#endif /* CONFIG_H_ */