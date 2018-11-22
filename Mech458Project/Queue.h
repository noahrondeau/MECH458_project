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
	unsigned int counter;
	bool isFerroMag;
	uint16_t reflectivity;
	ItemClass class;
} QueueElement;

#define DEFAULT_QUEUE_ELEM { .counter = 0, .isFerroMag = false, .reflectivity = MAX_ADC_VAL, .class = UNCLASSIFIED }

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
typedef volatile struct Queue
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

// QUEUE_Dequeue and QUEUE_Peak only return valid elements
// if they are not empty. USE QUEUE_IsEmpty or QUEUE_Size first!!!

void QUEUE_Init(Queue* q);
void QUEUE_Deinit(Queue* q);
void QUEUE_Enqueue(Queue* q, QueueElement elem);
QueueElement QUEUE_Dequeue(Queue* q);
QueueElement QUEUE_Peak(Queue* q);
int QUEUE_Size(Queue* q);
int QUEUE_IsEmpty(Queue* q);
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