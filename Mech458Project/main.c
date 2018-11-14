
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>

/* ====== FUNCTION PROTOTYPES ====== */
void Initialize();

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
	
	
	
	// ====== INIT CODE END   ======
	sei(); // turn on interrupts
}

ISR(BADISR_vect)
{
	
}


