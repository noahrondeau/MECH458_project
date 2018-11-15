

/* ====== LIBRARY INCLUDES ====== */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>

/* ====== USER INCLUDES ======*/

#include "config.h"
#include "DcMotor.h"
#include "ADC.h"
#include "FerroSensor.h"
#include "HallSensor.h"
#include "OpticalSensor.h"

/* ====== MAIN-LOCAL DEFINITIONS ====== */


/* ====== FUNCTION PROTOTYPES ====== */
void Initialize();

/* ====== GLOBAL SYSTEM RESOURCES ====== */

DcMotor			belt;
ADCHandle		adc;
FerroSensor		ferro;
HallSensor		hall;
OpticalSensor	s1_optic;
OpticalSensor	s2_optic;
OpticalSensor	exit_optic;


/* ====== MAIN FUNCTION ====== */

int main()
{
	Initialize();
	
	while(1)
	{
		
	}
	return 0;
}

void Initialize()
{
	cli(); // turn off interrupts
	// ====== INIT CODE START ======
	
	//DCMOTOR_Init(&belt);
	//ADC_Init();
	//FERRO_Init(&ferro);
	//HALL_Init(&hall);
	OPTICAL_Init(&s1_optic,S1_OPTICAL);
	OPTICAL_Init(&s2_optic,S2_OPTICAL);
	OPTICAL_Init(&exit_optic,EXIT_OPTICAL);
	
	DDRC = 0xFF;
	PORTC = 0x00;
	
	// ====== INIT CODE END   ======
	sei(); // turn on interrupts
}

ISR(BADISR_vect)
{
	
}


