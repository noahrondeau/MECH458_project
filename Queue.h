
#ifndef QUEUE_H
#define QUEUE_H

#include "config.h"

/* ====== Structure Definitions ====== */

// @name:	QueueElement
// @brief:	the stored data of a linked data structure
typedef struct QueueElement
{
	int item;
} QueueElement;

// @name:	QueueNode
// @brief:	a node in the linked data structure
struct QueueNode
{
	QueueElement* data;
	struct QueueNode* next;
	struct QueueNode* prev;
};
typedef struct QueueNode QueueNode;

// @name:	Queue
// @brief:	a linked queue data structure object
typedef struct Queue
{
	// Data Fields
	QueueNode* front;
	QueueNode* back;
	int size;

} Queue;

/* ====== Method Definitions ====== */

void QUEUE_init(Queue* q);
void QUEUE_deinit(Queue* q);
QueueNode* QUEUE_enqueue(Queue* q, QueueElement elem);
QueueElement* QUEUE_dequeue(Queue* q);
QueueElement* QUEUE_peak(Queue* q);
int QUEUE_size(Queue* q);
int QUEUE_isEmpty(Queue* q);
Queue* QUEUE_create(void);
void QUEUE_destroy(Queue** q);

#if UNITTEST_MODE == 1
#include "unittest.h"
void QUEUE_unitTest(void);
#endif

#endif