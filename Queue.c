

#include <stdlib.h>
#include "Queue.h"

#if MODE_ENABLED(LINKED_QUEUE_MODE)

void QUEUE_init(Queue* q)
{
	q->front = NULL;
	q->back = NULL;
	q->size = 0;
}

void QUEUE_deinit(Queue* q)
{
	while (q->front != NULL)
	{
		free(QUEUE_dequeue(q));
	}
}

QueueElement* QUEUE_enqueue(Queue* q, QueueElement elem)
{
	QueueNode* newNode = (QueueNode*)malloc(sizeof(QueueNode));
	newNode->data = (QueueElement*)malloc(sizeof(QueueElement));
	*(newNode->data) = elem;

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
	return (newNode->data);
}

QueueElement* QUEUE_dequeue(Queue* q)
{
	QueueElement* elem = NULL;

	//if not empty
	if (q->front != NULL)
	{
		elem = q->front->data;
		QueueNode* node = q->front;

		q->front = q->front->next;

		//if that was the last node reset back ptr
		if (q->front == NULL)
			q->back = NULL;

		free(node);
		q->size--;
	}
	return elem;
}

QueueElement* QUEUE_peak(Queue* q)
{
	QueueElement* elem = NULL;
	if (q->front)
		elem = q->front->data;
	return elem;
}

int QUEUE_size(Queue* q)
{
	return q->size;
}

int QUEUE_isEmpty(Queue* q)
{
	return (q->size == 0);
}

Queue* QUEUE_create(void)
{
	Queue* newQ = (Queue*)malloc(sizeof(Queue));
	QUEUE_init(newQ);
	return newQ;
}

void QUEUE_destroy(Queue** q)
{
	QUEUE_deinit(*q);
	free(*q);
	*q = NULL;
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

	QueueElement* old_array = q->array_start;
	q->array_start = new_array;
	q->front = q->array_start;
	q->back = (q->array_start + q->size - 1); 
	q->array_length *= 2;
	free(old_array);
}

void QUEUE_init(Queue* q)
{
	q->array_length = CIRCULAR_QUEUE_INIT_SIZE;
	q->array_start =
		(QueueElement*)malloc(CIRCULAR_QUEUE_INIT_SIZE*sizeof(QueueElement));

	q->array_end = q->array_start + CIRCULAR_QUEUE_INIT_SIZE;
	q->front = NULL;
	q->back = NULL;
	q->size = 0;
}

void QUEUE_deinit(Queue* q)
{
	while (q->front != NULL)
	{
		free(QUEUE_dequeue(q));
	}
}

QueueElement* QUEUE_enqueue(Queue* q, QueueElement elem)
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

QueueElement* QUEUE_dequeue(Queue* q)
{
	QueueElement* retval = NULL;

	if (q->front == NULL) // empty
		return retval;

	retval = (QueueElement*)malloc(sizeof(QueueElement));
	*retval = *(q->front);

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

QueueElement* QUEUE_peak(Queue* q)
{
	QueueElement* retval = NULL;

	if(q->front)
	{
		retval = (QueueElement*)malloc(sizeof(QueueElement));
		*retval = *(q->front);
	}

	return retval;
}

int QUEUE_size(Queue* q)
{
	return q->size;
}

int QUEUE_isEmpty(Queue* q)
{
	return (q->size == 0);
}

Queue* QUEUE_create(void)
{
	Queue* newQ = (Queue*)malloc(sizeof(Queue));
	QUEUE_init(newQ);
	return newQ;
}

void QUEUE_destroy(Queue** q)
{
	QUEUE_deinit(*q);
	free(*q);
	*q = NULL;
}



#endif //QUEUE_MODE



#if UNITTEST_MODE == 1
#include "unittest.h"
void QUEUE_unitTest(void)
{
	int count = 1;

	printf("Starting unit-tests on QUEUE\n");
	printf("Using %s\n", LINKED_QUEUE_MODE ? "Linked Queue": "Circular Queue");

	Queue* q = QUEUE_create();
	TEST(count, "Verify initial queue front", q->front == NULL);
	TEST(count, "Verify initial queue back", q->back == NULL);

	TEST(count, "Verify empty queue size",QUEUE_size(q) == 0);
	TEST(count, "Verify isEmpty returns true", QUEUE_isEmpty(q) >= 1);

	QueueElement* test_elem = QUEUE_dequeue(q);
	TEST(count, "Verify dequeueing empty queue returns NULL", test_elem == NULL);
	free(test_elem);

	test_elem = QUEUE_peak(q);
	TEST(count, "Verify peak on empty queue returns NULL", test_elem == NULL);

	QueueElement test_elem_1 = {
		.item = 1,
	};

	QUEUE_enqueue(q, test_elem_1);
	TEST(count, "Verify size after enqueue",QUEUE_size(q) == 1);

	QueueElement test_elem_2 = {
		.item = 2,
	};

	QueueElement test_elem_3 = {
		.item = 3,
	};

	QUEUE_enqueue(q, test_elem_2);
	QUEUE_enqueue(q, test_elem_2);

	test_elem = QUEUE_dequeue(q);
	TEST(count, "Verify dequeue order correct", test_elem->item == 1);
	free(test_elem);

	test_elem = QUEUE_peak(q);
	TEST(count, "Verify peak order correct", test_elem->item == 2);
	free(test_elem);

	QUEUE_deinit(q);
	TEST(count, "Verify deinit", q->front == NULL);
	TEST(count, "Verify deinit", q->back == NULL);

	QUEUE_destroy(&q);
	TEST(count, "Check that destroy works", q == NULL);
}

#endif //UNITTEST_MODE