/*
 * config.h
 *
 * Created: 2018-11-13 11:19:57 PM
 *  Author: ntron
 */ 

/* This is a configuration file which is shared everywhere in the system.
 * It contains literal values for many parameters in the system.
 * It also provides definitions for various globally useful types
 * This file allows configurations to be changed in one place only.
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

// these pre-processor definitions allow multiple build modes to be enabled or disabled

#define MODE_ENABLED(__mode__)	((__mode__) == 1)
#define MODE_DISABLED(__mode__)	((__mode__) == 0)

#define PROGRAM_MODE						(1)
#define UNITTEST_MODE						(!PROGRAM_MODE)
#define DEBUG_MODE 							(0)

#define LINKED_QUEUE_MODE					(0)
#define CIRCULAR_QUEUE_MODE					(!LINKED_QUEUE_MODE)

#define TWO_PHASE_COMMUTATION_MODE			(1)
#define ACCEL_MODE							(1)
#define S_CURVE_MODE						(1)
#define TRAP_MODE							(!S_CURVE_MODE)

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

/* ====== TRAY and STEPPER MOTOR CONFIG ====== */

#define STEPPER_DDR				(DDRA_REG)
#define STEPPER_PORT			(PORTA_REG)

#define TRAY_HOME_OFFSET				(10)		// how many steps CCW from the Hall sensor we consider to be "home"

#define TRAY_INTERRUPT_INIT_DELAY		(500)		//us
#define FIRST_ITEM_IN_QUEUE_DELAY		(0)			//ms
#define ITEM_READY_BEFORE_TRAY_DELAY	(5)			//ms
#define TRAY_READY_BEFORE_ITEM_DELAY	(100)		//ms

#if MODE_ENABLED(ACCEL_MODE) // use acceleration: this is for actual testing
	
	#define STEPPER_DELAY_MAX			(20000) // microseconds
	#define STEPPER_DELAY_MIN			(7500)  // microseconds
	#define STEPPER_ACCEL_RAMP			(6)		// step length
	#define CCW_Range					(30)
	#define	CW_Range					(30)
	
	#if MODE_ENABLED(S_CURVE_MODE) // use s-curve
		#define DELAY_PROFILE_COEFFS	{19000, 16000, 13000, 10189, 8569, 8132,}
	#elif MODE_ENABLED(TRAP_MODE) // use trapezoid
		#define DELAY_PROFILE_COEFFS	{20000, 19000, 18000, 17000, 16000, 15000, 14000, 13000, 12000, 11000, 10000, 9000,} // microsec
	#endif
#else // MODE_DISABLED(ACCEL_MODE) // mostly for testing stuff
	#define STEPPER_DELAY_MAX			(20000) // microseconds
#endif


#define MS_TO_US(__ms__) (1000*(__ms__))

/* ====== DC MOTOR CONFIG ====== */

#define DCMOTOR_PORTX			(PORTB_REG)
#define DCMOTOR_DDRX			(DDRB_REG)

#define DCMOTOR_SPEED			(90)

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

#define FERRO_PINX				(PINE_REG)
#define FERRO_DDRX				(DDRE_REG)
#define FERRO_PORTPIN			(5)

/* ====== SW1 PAUSE SWITCH CONFIG ====== */

#define PAUSE_PINX				(PINE_REG)
#define PAUSE_DDRX				(DDRE_REG)
#define PAUSE_PORTPIN			(7)

/* ====== SW2 RAMP-DOWN SWITCH CONFIG ====== */

#define RAMPDOWN_PINX			(PINE_REG)
#define RAMPDOWN_DDRX			(DDRE_REG)
#define RAMPDOWN_PORTPIN		(6)	

/* ====== RL SENSOR CONFIGS ====== */

#define LARGEST_UINT16_T ((uint16_t)(0xFFFF))

#define AVG_ALUMINIUM_VAL		(24.996k)
#define STDEV_ALUMINIUM			(9.7944k) // max 88

#define AVG_STEEL_VAL			(381.3667k)
#define STDEV_STEEL				(67.1773k) // max 558 min 194

#define AVG_WHITE_VAL			(934.0972k)
#define STDEV_WHITE				(19.2933k) // max 963 min 899

#define AVG_BLACK_VAL			(988.4819k)
#define STDEV_BLACK				(5.7411k) // max 1008 // min 974

#define METAL_CUTOFF_REFL		(700)
#define STEEL_ALUMINIUM_CUTOFF	(141)
#define BLACK_WHITE_CUTOFF		(968)

/* ====== SECOND ORDER BUTTERWORTH FILTER COEFFS ====== */
#define IIRB0					(0.0015K)
#define IIRB1					(0.0029K)
#define IIRB2					(0.0015K)
#define IIRA1					(1.8890K)
#define IIRA2					(-0.8949k)

/* ====== GLOBALLY USEFUL TYPEDEFS ====== */

// whether a sensor is actie low or high
typedef enum ActiveLevel
{
	ACTIVE_LOW = 0x00,
	ACTIVE_HIGH = 0x01,
} ActiveLevel;

// the state of a Finite state machine
typedef enum FsmState
{
	 RUN_STATE,
	 PAUSE_STATE,
	 RAMPDOWN_STATE, 
} FsmState;

// type for holding state data
typedef volatile struct FiniteStateMachine
{
	FsmState state;				// current state
	struct {					// saved state
		bool beltWasRunning;
		FsmState returnToState;
	} saved;
	bool rampDownInitFlag;
	bool rampDownEndFlag;
} FiniteStateMachine;

// type representing different materials
// the numeric values are used directly for tray targetting,
// indexing, and other numeric things to simplify logic
typedef enum ItemClass
{
	UNCLASSIFIED = 255, // bogus value, need to check everywhere!!!
	STEEL = 150,
	WHITE_PLASTIC = 100,
	ALUMINIUM = 50,
	BLACK_PLASTIC = 0,
}ItemClass;

#endif /* CONFIG_H_ */