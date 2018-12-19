/*
 * Queue.h
 *
 * Created: 2018-11-13 11:33:20 PM
 *  Author: ntron
 */ 

/* THIS DOCUMENT IMPLEMENTS TWO DIFFERENT QUEUES
 *
 * To select which you want to use, go to config.h and set the build mode
 * Options are Circular Queue or Linked Queue
 *
 * this is self-explanatory. The interface is the same, but the implementations
 * are different.
 * One uses a linked list structure, and the other a circular buffer structure
 * The circular buffer was ultimately chosen.
 * 
 * This library uses the stdatomic.h library, making calls to these functions
 * uninterruptible from main. They will execute atomically.
 */

#ifndef QUEUE_H_
#define QUEUE_H_


#include "config.h"

/* ====== Structure Definitions ====== */

// @name:	QueueElement
// @brief:	the stored data of a linked data structure
typedef struct QueueElement
{
	unsigned int counter;
	bool isFerroMag;
	uint16_t reflectivity;
	uint16_t sampleCount;
	ItemClass class;
} QueueElement;

#define DEFAULT_QUEUE_ELEM { .counter = 0, .isFerroMag = false, .reflectivity = LARGEST_UINT16_T, .class = UNCLASSIFIED }

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
	uint16_t size;

} Queue;

#elif MODE_ENABLED(CIRCULAR_QUEUE_MODE)


// @name:	Queue
// @brief:	a linked queue data structure object
typedef volatile struct Queue
{
	QueueElement* front;
	QueueElement* back;
	QueueElement* array_start;
	QueueElement* array_end;
	uint16_t array_length;
	uint16_t size;
} Queue;

#endif

/* ====== Method Definitions ====== */

// QUEUE_Dequeue and QUEUE_Peak only return valid elements
// if they are not empty. USE QUEUE_IsEmpty or QUEUE_Size first!!!

void QUEUE_Init(Queue* q);
void QUEUE_Deinit(Queue* q);
void QUEUE_Enqueue(Queue* q, QueueElement elem);
QueueElement QUEUE_Dequeue(Queue* q);
QueueElement QUEUE_Peak(Queue* q);
QueueElement QUEUE_PeakSecond(Queue* q);
uint16_t QUEUE_Size(Queue* q);
bool QUEUE_IsEmpty(Queue* q);
Queue* QUEUE_Create(void);
void QUEUE_Destroy(Queue** q);
//void QUEUE_PointerIncrement(Queue* q, QueueElement** q_ptr);
//void QUEUE_PointerDecrement(Queue* q, QueueElement** q_ptr);
QueueElement* QUEUE_BackPtr(Queue* q);


#if MODE_ENABLED(UNITTEST_MODE)
#include "unittest.h"
void QUEUE_UnitTest(void);
#endif




#endif /* QUEUE_H_ */