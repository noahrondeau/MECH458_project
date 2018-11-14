/*
 * Queue.h
 *
 * Created: 2018-11-13 11:33:20 PM
 *  Author: ntron
 */ 


#ifndef QUEUE_H_
#define QUEUE_H_


#include "config.h"

/* ====== Structure Definitions ====== */

// @name:	QueueElement
// @brief:	the stored data of a linked data structure
typedef struct QueueElement
{
	int item;
} QueueElement;

#define DEFAULT_QUEUE_ELEM { .item = 0 }

#if MODE_ENABLED(LINKED_QUEUE_MODE)

// @name:	QueueNode
// @brief:	a node in the linked data structure
struct QueueNode
{
	QueueElement data;
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

#elif MODE_ENABLED(CIRCULAR_QUEUE_MODE)


// @name:	Queue
// @brief:	a linked queue data structure object
typedef struct Queue
{
	QueueElement* front;
	QueueElement* back;
	QueueElement* array_start;
	QueueElement* array_end;
	int array_length;
	int size;
} Queue;

#endif

/* ====== Method Definitions ====== */

// QUEUE_dequeue and QUEUE_peak only return valid elements
// if they are not empty. USE QUEUE_isEmpty or QUEUE_size first!!!

void QUEUE_init(Queue* q);
void QUEUE_deinit(Queue* q);
void QUEUE_enqueue(Queue* q, QueueElement elem);
QueueElement QUEUE_dequeue(Queue* q);
QueueElement QUEUE_peak(Queue* q);
int QUEUE_size(Queue* q);
int QUEUE_isEmpty(Queue* q);
Queue* QUEUE_create(void);
void QUEUE_destroy(Queue** q);

#if MODE_ENABLED(UNITTEST_MODE)
#include "unittest.h"
void QUEUE_unitTest(void);
#endif




#endif /* QUEUE_H_ */