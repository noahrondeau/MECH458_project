/*
 * Queue.c
 *
 * Created: 2018-11-13 11:47:53 PM
 *  Author: ntron
 */ 

#include <util/atomic.h>
#include <stdlib.h>
#include "Queue.h"
#include "config.h"

#if MODE_ENABLED(LINKED_QUEUE_MODE)
/* LINKED QUEUE IMPLEMENTATIONS */

void QUEUE_Init(Queue* q)
{
	q->front = NULL;
	q->back = NULL;
	q->size = 0;
}

void QUEUE_Deinit(Queue* q)
{
	while (q->front != NULL)
	{
		QUEUE_Dequeue(q);
	}
}

void QUEUE_Enqueue(Queue* q, QueueElement elem)
{
	QueueNode* newNode = (QueueNode*)malloc(sizeof(QueueNode));
	newNode->data = elem;
	newNode->next = NULL;
	
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		newNode->prev = q->back;

		// if was empty
		if (q->front == NULL)
			q->front = newNode;
		//if was not empty
		else
			q->back->next = newNode;

		q->back = newNode;
		q->size++;
	}
}

QueueElement QUEUE_Dequeue(Queue* q)
{
	QueueElement retval = DEFAULT_QUEUE_ELEM;
	
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		//if not empty
		if (q->front != NULL)
		{
			retval = q->front->data;
			QueueNode* node = q->front;

			q->front = q->front->next;

			//if that was the last node reset back ptr
			if (q->front == NULL)
				q->back = NULL;

			free(node);
			q->size--;
		}
	}
	return retval;
}

QueueElement QUEUE_Peak(Queue* q)
{
	QueueElement elem = DEFAULT_QUEUE_ELEM;
	
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		if (q->front)
			elem = q->front->data;
	}
	return elem;
}

QueueElement QUEUE_PeakSecond(Queue* q)
{
	QueueElement elem = DEFAULT_QUEUE_ELEM;
	
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		if (q->front && q->front->next)
			elem = q->front->next->data;
	}
	return elem;
}

uint16_t QUEUE_Size(Queue* q)
{
	uint16_t ret;
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		 ret = (q->size);
	}
	return ret;
}

bool QUEUE_IsEmpty(Queue* q)
{
	bool ret;
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		if (q->size == 0)
			ret = true;
		else
			ret =  false;
	}
	return ret;
}

Queue* QUEUE_Create(void)
{
	Queue* newQ = (Queue*)malloc(sizeof(Queue));
	QUEUE_Init(newQ);
	return newQ;
}

void QUEUE_Destroy(Queue** q)
{
	QUEUE_Deinit(*q);
	free(*q);
	*q = NULL;
}

QueueElement* QUEUE_BackPtr(Queue* q)
{
	QueueElement* ptr;
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		ptr = &(q->back->data);
	}
	return ptr;
}

#elif MODE_ENABLED(CIRCULAR_QUEUE_MODE)
/* CIRCULAR QUEUE IMPLEMENTTIONS */

#define CIRCULAR_QUEUE_INIT_SIZE (50)

// @name: QUEUE_enlarge
// @brief: enlarges the dynamically allocated queue
//	to be used if the queue is full
//	this is the only O(N) operation in this Queue library
//	it also "reorders" the queue as a byproduct
//	also, the function assumes the queue is not empty,
//	since it will never be called in that case
void QUEUE_enlarge(Queue* q)
{
	// allocate new array that is twice the size of the previous
	QueueElement* new_array =
	(QueueElement*)malloc(2*(q->array_length)*sizeof(QueueElement));

	// copy data from old array to new array
	QueueElement* aux = q->front;
	for (int i = 0; i < q->size; i++)
	{
		new_array[i] = *aux;

		if (aux == q->array_end)
		aux = q->array_start;
		else
		aux++;
	}

	q->array_length *= 2;
	QueueElement* old_array = q->array_start;
	q->array_start = new_array;
	q->array_end = q->array_start + q->array_length - 1;
	q->front = q->array_start;
	q->back = (q->array_start + q->size - 1);
	free(old_array);
}

void QUEUE_Init(Queue* q)
{
	q->array_length = CIRCULAR_QUEUE_INIT_SIZE;
	q->array_start =
	(QueueElement*)malloc(CIRCULAR_QUEUE_INIT_SIZE*sizeof(QueueElement));

	q->array_end = q->array_start + CIRCULAR_QUEUE_INIT_SIZE - 1;
	q->front = NULL;
	q->back = NULL;
	q->size = 0;
}
// reset queue
void QUEUE_Deinit(Queue* q)
{
	while (q->front != NULL)
	{
		QUEUE_Dequeue(q);
	}
}

// enqueue an item
void QUEUE_Enqueue(Queue* q, QueueElement elem)
{
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
			//empty case
		if (q->back == NULL)
		{
			q->front = q->array_start;
			q->back = q->array_start;
			*(q->front) = elem;
		}
		// not empty case
		else
		{
			// array full
			if (q->size == q->array_length)
			QUEUE_enlarge(q);

			if (q->back == q->array_end)
			q->back = q->array_start;
			else
			q->back++;

			*(q->back) = elem;
		}
		q->size++;
	}
}

// dequeu an item
QueueElement QUEUE_Dequeue(Queue* q)
{
	QueueElement retval = DEFAULT_QUEUE_ELEM;
	
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		if (q->front != NULL) // not empty
		{
			retval = *(q->front);

			if (q->front == q->back)
			//this was the last element
			{
				q->front = NULL;
				q->back = NULL;
			}
			else // this was not the last element
			{
				if (q->front == q->array_end)
				q->front = q->array_start;
				else
				q->front++;
			}

			q->size--;
		}
	}

	return retval;
}

// peak at the first item without dequeueing
QueueElement QUEUE_Peak(Queue* q)
{
	QueueElement retval = DEFAULT_QUEUE_ELEM;
	
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		if(q->front)
			retval = *(q->front);
	}
	return retval;
}

// peak at the second item
QueueElement QUEUE_PeakSecond(Queue* q)
{
	QueueElement retval = DEFAULT_QUEUE_ELEM;
	
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		if(q->size > 1)
		{
			if (q->front == q->array_end)
				retval = *(q->array_start);
			else if( q->front >= q->array_start)
				retval = *(q->front + 1);
				
		}
	}
	return retval;
}

// get the size of the queue
uint16_t QUEUE_Size(Queue* q)
{
	uint16_t ret;
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		ret = q->size;
	}
	return ret;
}

// check if the queue is empty
bool QUEUE_IsEmpty(Queue* q)
{
	bool ret;
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		if (q->size == 0)
			ret = true;
		else
			ret = false;
	}
	return ret;
}

// factory function for a new queue
Queue* QUEUE_Create(void)
{
	Queue* newQ = (Queue*)malloc(sizeof(Queue));
	QUEUE_Init(newQ);
	return newQ;
}

// Destructor for the queue
void QUEUE_Destroy(Queue** q)
{
	QUEUE_Deinit(*q);
	free(*(void**)q);
	*q = NULL;
}

// get a pointer to the back of the queue
QueueElement* QUEUE_BackPtr(Queue* q)
{
	QueueElement* ptr;
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		ptr = q->back;
	}
	return ptr;
}
#endif //QUEUE_MODE



#if UNITTEST_MODE == 1
#include "unittest.h"
void QUEUE_UnitTest(void)
{
	int count = 1;

	printf("Starting unit-tests on QUEUE\n");
	printf("Using %s\n", LINKED_QUEUE_MODE ? "Linked Queue": "Circular Queue");

	Queue* q = QUEUE_Create();
	TEST(count, "Verify initial queue front", q->front == NULL);
	TEST(count, "Verify initial queue back", q->back == NULL);

	TEST(count, "Verify empty queue size",QUEUE_Size(q) == 0);
	TEST(count, "Verify isEmpty returns true", QUEUE_IsEmpty(q) >= 1);

	QueueElement test_elem = QUEUE_Dequeue(q);
	TEST(count, "Verify dequeueing empty queue returns default", test_elem.item == 0);

	test_elem = QUEUE_Peak(q);
	TEST(count, "Verify peak on empty queue returns default", test_elem.item == 0);

	QueueElement test_elem_1 = {
		.item = 1,
	};

	QUEUE_Enqueue(q, test_elem_1);
	TEST(count, "Verify size after enqueue",QUEUE_Size(q) == 1);

	QueueElement test_elem_2 = {
		.item = 2,
	};

	QueueElement test_elem_3 = {
		.item = 3,
	};

	QUEUE_Enqueue(q, test_elem_2);
	QUEUE_Enqueue(q, test_elem_2);

	test_elem = QUEUE_Dequeue(q);
	TEST(count, "Verify dequeue order correct", test_elem.item == 1);

	test_elem = QUEUE_Peak(q);
	TEST(count, "Verify peak order correct", test_elem.item == 2);

	QUEUE_Deinit(q);
	TEST(count, "Verify deinit", q->front == NULL);
	TEST(count, "Verify deinit", q->back == NULL);

	#if MODE_ENABLED(CIRCULAR_QUEUE_MODE)

	QUEUE_Init(q);
	for(int i = 0; i < CIRCULAR_QUEUE_INIT_SIZE; i++)
	QUEUE_Enqueue(q, (QueueElement){ .item = i });
	for(int i = 0; i < CIRCULAR_QUEUE_INIT_SIZE/2; i++)
	QUEUE_Dequeue(q);

	TEST(count, "Check that circular wrapping works", q->size == 25);
	TEST(count, "Check that circular wrapping works", QUEUE_Peak(q).item == 25);
	TEST(count, "Check that circular wrapping works", (q->front == q->array_start + CIRCULAR_QUEUE_INIT_SIZE/2));
	TEST(count, "Check that circular wrapping works", q->back == q->array_end);

	for(int i = CIRCULAR_QUEUE_INIT_SIZE; i < CIRCULAR_QUEUE_INIT_SIZE*3/2; i++)
	QUEUE_Enqueue(q, (QueueElement){ .item = i });

	TEST(count, "Check that circular wrapping works", q->size == CIRCULAR_QUEUE_INIT_SIZE);
	TEST(count, "Check that circular wrapping works", q->back == q->array_start + CIRCULAR_QUEUE_INIT_SIZE/2 - 1);
	TEST(count, "Check that circular wrapping works", q->back->item == CIRCULAR_QUEUE_INIT_SIZE*3/2 -1);

	for (int i = CIRCULAR_QUEUE_INIT_SIZE*3/2; i < 2*CIRCULAR_QUEUE_INIT_SIZE; i++)
	QUEUE_Enqueue(q, (QueueElement){ .item = i });
	

	TEST(count, "Check that queue enlargement works", q->size == 3*CIRCULAR_QUEUE_INIT_SIZE/2);
	TEST(count, "Check that queue enlargement works", q->array_length == 2*CIRCULAR_QUEUE_INIT_SIZE);
	TEST(count, "Check that queue enlargement works", q->front == q->array_start);
	TEST(count, "Check that queue enlargement works", q->back == q->array_start + 3*CIRCULAR_QUEUE_INIT_SIZE/2 - 1);
	TEST(count, "Check that queue enlargement works", q->array_end == q->array_start + 2*CIRCULAR_QUEUE_INIT_SIZE - 1);

	for (int i = 0; i < q->size; i++)
	{
		TEST(count, "Verify queue enlargement preserved order", QUEUE_Dequeue(q).item == i + 25);
	}

	#endif // circular queue mode

	QUEUE_Destroy(&q);
	TEST(count, "Check that destroy works", q == NULL);

}

#endif //UNITTEST_MODE