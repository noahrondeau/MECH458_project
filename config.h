
#ifndef CONFIG_H
#define CONFIG_H

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
	#define DEBUG(__t__) do{ __t__ ;}while(0)
#else
	#define DEBUG(__t__)
#endif

/* ====== GPIO PORT REG DEFS ====== */

typedef volatile uint8_t* GPIOPort;

#define PINA_REG		((GPIOPort)((0x00) + (__SFR_OFFSET)))
#define DDRA_REG		((GPIOPort)((0x01) + (__SFR_OFFSET)))
#define PORTA_REG		((GPIOPort)((0x02) + (__SFR_OFFSET)))

#define PINB_REG		((GPIOPort)((0x03) + (__SFR_OFFSET)))
#define DDRB_REG		((GPIOPort)((0x04) + (__SFR_OFFSET)))
#define PORTB_REG		((GPIOPort)((0x05) + (__SFR_OFFSET)))

#define PINC_REG		((GPIOPort)((0x06) + (__SFR_OFFSET)))
#define DDRC_REG		((GPIOPort)((0x07) + (__SFR_OFFSET)))
#define PORTC_REG		((GPIOPort)((0x08) + (__SFR_OFFSET)))

#define PIND_REG		((GPIOPort)((0x09) + (__SFR_OFFSET)))
#define DDRD_REG		((GPIOPort)((0x0A) + (__SFR_OFFSET)))
#define PORTD_REG		((GPIOPort)((0x0B) + (__SFR_OFFSET)))

#define PINE_REG		((GPIOPort)((0x0C) + (__SFR_OFFSET)))
#define DDRE_REG		((GPIOPort)((0x0D) + (__SFR_OFFSET)))
#define PORTE_REG		((GPIOPort)((0x0E) + (__SFR_OFFSET)))

#define PINF_REG		((GPIOPort)((0x0F) + (__SFR_OFFSET)))
#define DDRF_REG		((GPIOPort)((0x10) + (__SFR_OFFSET)))
#define PORTF_REG		((GPIOPort)((0x11) + (__SFR_OFFSET)))

/* ====== STEPPER MOTOR CONFIG ====== */

#define STEPPER_DDR (DDRA_REG)
#define STEPPER_PORT (PORTA_REG)

/* ====== OTHER CONFIGS ====== */

typedef enum bool
{
	false = 0,
	true = 1,
} bool;

#endif