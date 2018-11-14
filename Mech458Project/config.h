/*
 * config.h
 *
 * Created: 2018-11-13 11:19:57 PM
 *  Author: ntron
 */ 


#ifndef CONFIG_H_
#define CONFIG_H_


#include <stdint.h>
#include <avr/io.h>

/* ====== BUILD MODE CONFIG ====== */
#define PROGRAM_MODE	(1)
#define UNITTEST_MODE	(!PROGRAM_MODE)
#define DEBUG_MODE 		(0)

#define LINKED_QUEUE_MODE	(0)
#define CIRCULAR_QUEUE_MODE	(!LINKED_QUEUE_MODE)

#define MODE_ENABLED(__mode__)	((__mode__) == 1)
#define MODE_DISABLED(__mode__)	((__mode__) == 0)

#if MODE_ENABLED(DEBUG_MODE)
#include <stdio.h>
#define DEBUG_ONLY(__t__) do{ __t__ ;}while(0)
#else
#define DEBUG_ONLY(__t__)
#endif

/* ====== GPIO PORT REG DEFS ====== */

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

#define STEPPER_DDR (DDRA_REG)
#define STEPPER_PORT (PORTA_REG)

/* ====== HALL SENSOR CONFIG ====== */

#define HALL_ACTIVE ((uint8_t)(0x00))
#define HALL_PINX (PIND_REG)
#define HALL_DDRX (DDRD_REG)
#define HALL_PORTPIN (3)

/* ====== OPTICAL SENSOR CONFIG ====== */

#define OPTICAL1_PINX (PIND_REG)
#define OPTICAL1_DDRX (DDRD_REG)
#define OPTICAL1_PORTPIN (0)
#define OPTICAL1_ACTIVE (0x00)

#define OPTICAL2_PINX (PIND_REG)
#define OPTICAL2_DDRX (DDRD_REG)
#define OPTICAL2_PORTPIN (1)
#define OPTICAL2_ACTIVE (0b00000010)

/* ====== FERRO SENSOR CONFIG ====== */

#define FERRO_PINX (PINA_REG)
#define FERRO_DDRX (DDRA_REG)
#define FERRO_PORTPIN (3)
#define FERRO_ACTIVE (0x00)

/* ====== EXIT GATE CONFIG ====== */

#define EXIT_PINX (PIND_REG)
#define EXIT_DDRX (DDRD_REG)
#define EXIT_PORTPIN (2)
#define EXIT_ACTIVE (0x00)

/* ====== GLOBALLY USEFUL TYPEDEFS ====== */

typedef enum bool
{
	false = 0,
	true = 1,
} bool;


typedef enum ActiveLevel
{
	ACTIVE_LOW = 0x00,
	ACTIVE_HIGH = 0x01,
} ActiveLevel;




#endif /* CONFIG_H_ */