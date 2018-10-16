


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
#include <stdio.h>
int test_counter = 1;
#define TEST(__desc__, __exp__) \
	printf("TEST #%d: %s\n\t--Testing %s:", test_counter, __desc__, #__exp__ ); \
	if (__exp__) \
	{ \
		printf("\tPASSED\n\n"); \
		test_counter++; \
	} \
	else \
	{ \
		printf("\tFAILED\n\n"); \
		return 1; \
	}

/*UNITTEST*/ int main(void) /*UNITTEST*/
{
	printf("Starting unit-tests\n\n");

	Queue* q = QUEUE_create();
	TEST("Verify initial queue front", q->front == NULL);
	TEST("Verify initial queue back", q->back == NULL);

	TEST("Verify empty queue size",QUEUE_size(q) == 0);
	TEST("Verify isEmpty returns true", QUEUE_isEmpty(q) >= 1);

	QueueElement* test_elem = QUEUE_dequeue(q);
	TEST("Verify dequeueing empty queue returns NULL", test_elem == NULL);
	free(test_elem);

	test_elem = QUEUE_peak(q);
	TEST("Verify peak on empty queue returns NULL", test_elem == NULL);

	QueueElement test_elem_1 = {
		.item = 1,
	};

	QUEUE_enqueue(q, test_elem_1);
	TEST("Verify size after enqueue",QUEUE_size(q) == 1);

	QueueElement test_elem_2 = {
		.item = 2,
	};

	QueueElement test_elem_3 = {
		.item = 3,
	};

	QUEUE_enqueue(q, test_elem_2);
	QUEUE_enqueue(q, test_elem_2);

	test_elem = QUEUE_dequeue(q);
	TEST("Verify dequeue order correct", test_elem->item == 1);
	free(test_elem);

	test_elem = QUEUE_peak(q);
	TEST("Verify peak order correct", test_elem->item == 2);
	free(test_elem);

	QUEUE_deinit(q);
	TEST("Verify deinit", q->front == NULL);
	TEST("Verify deinit", q->back == NULL);

	QUEUE_destroy(&q);
	TEST("Check that destroy works", q == NULL);

	return 0;
}
#endif

/* ====== FUNCTION DEFINITIONS ====== */
