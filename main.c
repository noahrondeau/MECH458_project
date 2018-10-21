

/* ====== STANDARD INCLUDES ====== */

#include <stdlib.h>

/* ====== AVR INCLUDES ====== */

/* ====== USER INCLUDES ======*/
#include "config.h"
#include "Queue.h"

/* ====== GLOBAL DEFINES ====== */

/* ====== GLOBAL VARIABLES ====== */


/* ====== FUNCTION PROTOTYPES ====== */

/* ====== MAIN PROGRAM ====== */
#if MODE_ENABLED(PROGRAM_MODE)

int main(void)
{
	DEBUG(printf("HELLO WORLD\n"));
	return 0;
}

/* ====== UNITTESTS ======*/
#elif MODE_ENABLED(UNITTEST_MODE)
#include "unittest.h"
/*UNITTEST*/ int main(void) /*UNITTEST*/
{
	UNITTEST_addTest(&QUEUE_unitTest);
	UNITTEST_runTests();
	return 0;
}
#endif

/* ====== FUNCTION DEFINITIONS ====== */
