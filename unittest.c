
#include "unittest.h"
#include <stdio.h>

#define MAX_TESTS (10)

struct UnitTests
{
	int num_tests;
	void (*tests[MAX_TESTS])(void);
};

static struct UnitTests unitTests = {.num_tests = 0};

void UNITTEST_addTest(void (*test)(void))
{
	if (unitTests.num_tests >= MAX_TESTS)
	{
		printf("\nError: unable to add tests -- max tests reached\n");
		return;
	}

	unitTests.tests[unitTests.num_tests] = test;
	unitTests.num_tests++;
}

void UNITTEST_runTests(void)
{
	int index;
	for (index = 0; index < unitTests.num_tests; index++)
	{
		unitTests.tests[index]();
	}
}

