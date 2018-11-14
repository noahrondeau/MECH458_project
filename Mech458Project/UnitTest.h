/*
 * UnitTest.h
 *
 * Created: 2018-11-13 11:37:59 PM
 *  Author: ntron
 */ 


#ifndef UNITTEST_H_
#define UNITTEST_H_

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



#endif /* UNITTEST_H_ */