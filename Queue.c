

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