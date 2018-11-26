/*
 * Queue.c
 *
 * Created: 2018-11-13 11:47:53 PM
 *  Author: ntron
 */ 


#include <stdlib.h>
#include "Queue.h"

#if MODE_ENABLED(LINKED_QUEUE_MODE)

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

QueueElement QUEUE_Dequeue(Queue* q)
{
	QueueElement retval = DEFAULT_QUEUE_ELEM;

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
	return retval;
}

QueueElement QUEUE_Peak(Queue* q)
{
	QueueElement elem = DEFAULT_QUEUE_ELEM;
	if (q->front)
		elem = q->front->data;
	return elem;
}

uint16_t QUEUE_Size(Queue* q)
{
	return q->size;
}

bool QUEUE_IsEmpty(Queue* q)
{
	if (q->size == 0)
		return true;
	else
		return false;
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
	return &(q->back->data);
}

#elif MODE_ENABLED(CIRCULAR_QUEUE_MODE)

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

void QUEUE_Deinit(Queue* q)
{
	while (q->front != NULL)
	{
		QUEUE_Dequeue(q);
	}
}

void QUEUE_Enqueue(Queue* q, QueueElement elem)
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

QueueElement QUEUE_Dequeue(Queue* q)
{
	QueueElement retval = DEFAULT_QUEUE_ELEM;

	if (q->front == NULL) // empty
		return retval;

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

	return retval;
}

QueueElement QUEUE_Peak(Queue* q)
{
	QueueElement retval = DEFAULT_QUEUE_ELEM;

	if(q->front)
	{
		retval = *(q->front);
	}

	return retval;
}

uint16_t QUEUE_Size(Queue* q)
{
	return q->size;
}

bool QUEUE_IsEmpty(Queue* q)
{
	if (q->size == 0)
		return true;
	else 
		return false;
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
	free(*(void**)q);
	*q = NULL;
}

/* @brief: increment an auxiliary pointer to the next item in the queue
 * Makes pointer NULL if we increment when at the end of the queue.
 */
/*
void QUEUE_PointerIncrement(Queue* q, QueueElement** q_ptr)
{
	if (q_ptr == NULL) return; // invalid argument

	if ((*q_ptr) == NULL)
		return; // invalid pointer
		
	if ( QUEUE_IsEmpty(q) )
	{
		*q_ptr = NULL;
		return; // empty case where q front and back are NULL
	}
		
	if ( (*q_ptr) == q->back )
	{
		*q_ptr = NULL;
		return; // case where we are at the end and make NULL
	}
	
	if (q->front < q->back) // queue is continuous block
	{
		if ( (*q_ptr) < q->front || (*q_ptr) > q->back )
		{
			*q_ptr = NULL;
			return; // pointer not  referencing valid element
		}
		
		q_ptr++;
	}
	else // queue is wrapped around
	{
		if ((*q_ptr) < q->front && (*q_ptr) > q->back )
		{
			*q_ptr = NULL;
			return; // pointer not  referencing valid element
		}
		
		if ( (*q_ptr) == q->array_end )
			(*q_ptr) = q->array_start;
		else
			(*q_ptr)++;
	}
}*/
/* @brief: decrement an auxiliary pointer to the previous item in the queue
 * this call is unsafe. only call if you are certain that there is another item in the queue before it,
 * since otherwise it does nothing.
 *//*
void QUEUE_PointerDecrement(Queue* q, QueueElement** q_ptr)
{
	if ( QUEUE_IsEmpty(q) )
		return; // empty case where q front and back are NULL

	if ( QUEUE_Size(q) == 1 )
		return; // case where can't decrement, and both q->front == q->back

	if ( (*q_ptr) == q->front )
		return; // case where we can't decrement, we are at the start

	if (q->front < q->back) // queue is continuous block
	{
		q_ptr--;
	}
	else // queue is wrapped around
	{
		if ( (*q_ptr) == q->array_start )
			(*q_ptr) = q->array_end;
		else
			(*q_ptr)--;
	}	
}*/

QueueElement* QUEUE_BackPtr(Queue* q)
{
	return q->back;
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