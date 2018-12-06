

/* ====== LIBRARY INCLUDES ====== */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <stdio.h>

/* ====== USER INCLUDES ======*/

#include "config.h"
#include "LedBank.h"
#include "SysClk.h"
#include "DcMotor.h"
#include "ADC.h"
#include "OpticalSensor.h"
#include "Timer.h"
#include "PushButton.h"
#include "Queue.h"
#include "Tray.h"
#include "Filter.h"
#include "uart.h"

/* ====== MAIN-LOCAL DEFINITIONS ====== */


/* ====== FUNCTION PROTOTYPES ====== */
void Initialize();

/* ====== GLOBAL SYSTEM RESOURCES ====== */

// system resource handles, includes both internal and external periphs
// all these types are typedef'd as volatile, don't need to repeat here
DcMotor			belt;
ADCHandle		adc;
OpticalSensor	s2_optic;
PushButton		pauseButton;
Queue*			itemQ;

// sensor stage 2 stats singleton struct
volatile struct {
	uint16_t minReflectivity;
	uint32_t sampleCount;
	uint32_t itemCount;
	unsigned long accum avgSampleCount;
} Stage2 = {
	.minReflectivity = LARGEST_UINT16_T,
	.sampleCount = 0,
	.itemCount = 0,
	.avgSampleCount = 0.0k,
};

enum
{
	ALUMINIUM_STATE=0,
	STEEL_STATE,
	WHITE_STATE,
	BLACK_STATE,
	END_STATE,
} MaterialState;

bool breakSignal = false;



/* ====== MAIN FUNCTION ====== */

int main()
{
	Initialize();
	TIMER1_DelayMs(2000);
	DCMOTOR_Run(&belt,DCMOTOR_SPEED);
	
	MaterialState = ALUMINIUM_STATE;
	
	while(true)
	{
		LED_Set(0x000);
		QUEUE_Deinit(itemQ);
		TIMER1_DelayMs(1000);
		
		switch(MaterialState)
		{
		case ALUMINIUM_STATE:
			LED_On(0);
			TIMER1_DelayMs(1000);
			UART_SendString("ALUMINIUM\r\n");
			break;
		case STEEL_STATE:
			LED_On(1);
			TIMER1_DelayMs(1000);
			UART_SendString("STEEL\r\n");
			break;
		case WHITE_STATE:
			LED_On(2);
			TIMER1_DelayMs(1000);
			UART_SendString("WHITE\r\n");
			break;
		case BLACK_STATE:
			LED_On(3);
			TIMER1_DelayMs(1000);
			UART_SendString("BLACK\r\n");
			break;
		case END_STATE:
			Stage2.avgSampleCount = ((unsigned long accum)Stage2.sampleCount) / ((unsigned long accum)Stage2.itemCount);
			LED_Set((uint16_t)Stage2.avgSampleCount);
			UART_SendString("AVERAGE_SAMPLE_COUNT\r\n");
			char sendString[20];
			sprintf(sendString, "%u\r\n", (uint16_t)Stage2.avgSampleCount);
			UART_SendString(sendString);
			UART_SendString("END\r\n");
			LED_Set(0xFF);
			TIMER1_DelayMs(1000);
			LED_Set(0b10101010);
			TIMER1_DelayMs(1000);
			LED_Set(0xFF);
			TIMER1_DelayMs(1000);
			return 0;
			
		}
	
		
		while(!breakSignal)
		{
			if(!QUEUE_IsEmpty(itemQ))
			{
				uint16_t sendval = QUEUE_Dequeue(itemQ).reflectivity;
				//LED_Set(sendval)
				char sendString[10]; // ten bit numbers go up to 4 digits in decimal. need fifth for '\n' char, sixth for '\0'
				sprintf(sendString, "%u\r\n", sendval);
				UART_SendString(sendString);
			}
		}
		
		ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
		{
			breakSignal = false;
		}
	}
	
	return 0;
}

void Initialize()
{
	cli(); // turn off interrupts
	// clear EXTI interrupt enables just in case of weird startup state
	EIMSK = 0x00;
	EICRA = 0x00;
	EICRB = 0x00;
	// ====== INIT CODE START ======
	// Initialize all required resources and peripherals
	SYSCLK_Init();
	LED_Init();
	TIMER1_DelayInit();
	TIMER3_DelayInit();
	DCMOTOR_Init(&belt);
	ADC_Init(&adc, ADC_PRESCALE_128);
	OPTICAL_Init(&s2_optic,S2_OPTICAL);
	BUTTON_Init(&pauseButton, PAUSE_BUTTON);
	UART_Init();
	itemQ = QUEUE_Create();
	FILTER_InitReset(1023.0K); // initialize to most likely first value;
	// in the future, could do an ADC run and set to the average value of the background found
	// perhaps in an ADC_Calibrate function
	
	// ====== INIT CODE END   ======
	sei(); // turn on interrupts
}




/* ====== INTERRUPT SERVICE ROUTINES ====== */

// ISR for S2_OPTICAL
ISR(INT2_vect)
{
	// poll sensor state to make sure not spurious
	
	if (OPTICAL_IsBlocked(&s2_optic))
	{
		Stage2.minReflectivity = LARGEST_UINT16_T; // reset to default reflectivity
		FILTER_InitReset(1023.0K);
		ADC_StartConversion(&adc);
	}
}

// ISR for PAUSE button
ISR(INT7_vect)
{
	// Debounce
	// We should probably set up a new different timer for this
	// Since this one will be used for the stepper motor
	if (MaterialState != END_STATE)
		MaterialState++;
		
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		breakSignal = true;
	}
	breakSignal = true;
}

ISR(ADC_vect)
{
	ADC_ReadConversion(&adc);
	Stage2.sampleCount++;
	
	accum fx_out = Filter((accum)(adc.result));
	uint16_t u_out;
	
	if ( fx_out < 0.0K ) u_out = 0;
	else if (fx_out > 1023.0K) u_out = 1023;
	else u_out = (uint16_t)fx_out;
	
	if (u_out < Stage2.minReflectivity)
	Stage2.minReflectivity = u_out;
	
	if (OPTICAL_IsBlocked(&s2_optic))
	{
		ADC_StartConversion(&adc);
	}
	else
	{	
		Stage2.itemCount++;
		QueueElement newItem;
		newItem.reflectivity = Stage2.minReflectivity;
		QUEUE_Enqueue(itemQ, newItem);
	}
}

/*
ISR(TIMER3_COMPB_vect)
{
	
	LED_Toggle(1);
	TCNT3 = 0x0000;			//reset counter
	TIFR3 |= _BV(OCF3B);	//Clear interrupt flag begin counting
	
	PORTC = 0xFF;
}
*/

ISR(BADISR_vect)
{
	while(1)
	{
		LED_Set( 0b01010101);
		TIMER1_DelayMs(500);
		LED_Set( 0b10101010);
		TIMER1_DelayMs(500);
	}
}


