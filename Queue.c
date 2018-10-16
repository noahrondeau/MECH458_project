

#include <stdlib.h>
#include "Queue.h"

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

QueueNode* QUEUE_enqueue(Queue* q, QueueElement elem)
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
	return newNode;
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

#if UNITTEST_MODE == 1
#include "unittest.h"
void QUEUE_unitTest(void)
{
	int count = 1;

	printf("Starting unit-tests on QUEUE\n\n");

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
#endif