
#ifndef UNITTEST_H
#define UNITTEST_H

#include <stdio.h>

#define TEST(__num__, __desc__, __exp__) \
	printf("TEST #%d: %s\n\t--Testing %s:", __num__, __desc__, #__exp__ ); \
	if (__exp__) \
	{ \
		printf("\tPASSED\n\n"); \
		__num__++; \
	} \
	else \
	{ \
		printf("\tFAILED\n\n"); \
		return; \
	}

/* ====== PROTOTYPES ====== */

void UNITTEST_runTests(void);
void UNITTEST_addTest(void (*test)(void));
#endif