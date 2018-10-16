


/* ====== GLOBAL RUN-MODE DEFINITIONS ====== */

#define UNITTEST_MODE (1)
#define DEBUG_MODE (0)

/* ====== INCLUDES ====== */

#include "Queue.h"
#include <stdlib.h>


/* ====== GLOBAL DEFINES ====== */

#if DEBUG_MODE == 1
	#include <stdio.h>
	#define DEBUG(__t__) do{ __t__ ;}while(0)
#else
	#define DEBUG(__t__)
#endif

/* ====== GLOBAL VARIABLES ====== */


/* ====== FUNCTION PROTOTYPES ====== */

/* ====== MAIN PROGRAM ====== */
#if UNITTEST_MODE == 0

int main(void)
{
	return 0;
}

/* ====== UNITTESTS ======*/
#else
#include "unittest.h"
/*UNITTEST*/ int main(void) /*UNITTEST*/
{
	UNITTEST_addTest(&QUEUE_unitTest);
	UNITTEST_runTests();
	return 0;
}
#endif

/* ====== FUNCTION DEFINITIONS ====== */
