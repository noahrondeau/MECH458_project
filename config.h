
#ifndef CONFIG_H
#define CONFIG_H

#include <stdint.h>

/* ====== BUILD MODE CONFIG ====== */
#define PROGRAM_MODE	(1)
#define UNITTEST_MODE	(!PROGRAM_MODE)
#define DEBUG_MODE 		(0)

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

#define PINA_REG		((GPIOPort)(0x00))
#define DDRA_REG		((GPIOPort)(0x01))
#define PORTA_REG		((GPIOPort)(0x02))

#define PINB_REG		((GPIOPort)(0x03))
#define DDRB_REG		((GPIOPort)(0x04))
#define PORTB_REG		((GPIOPort)(0x05))

#define PINC_REG		((GPIOPort)(0x06))
#define DDRC_REG		((GPIOPort)(0x07))
#define PORTC_REG		((GPIOPort)(0x08))

#define PIND_REG		((GPIOPort)(0x09))
#define DDRD_REG		((GPIOPort)(0x0A))
#define PORTD_REG		((GPIOPort)(0x0B))

#define PINE_REG		((GPIOPort)(0x0C))
#define DDRE_REG		((GPIOPort)(0x0D))
#define PORTE_REG		((GPIOPort)(0x0E))

#define PINF_REG		((GPIOPort)(0x0F))
#define DDRF_REG		((GPIOPort)(0x10))
#define PORTF_REG		((GPIOPort)(0x11))

/* ====== STEPPER MOTOR CONFIG ====== */

#define STEPPER_DDR (DDRA_REG)
#define STEPPER_PORT (PORTA_REG)

/* ====== OTHER CONFIGS ====== */

#endif