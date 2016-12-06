#include <stdlib.h>
#include <stdio.h>

/* a link in the queue, holds the info and point to the next Node*/
typedef struct {
	char byte;
} DATA;

typedef struct Node_t {
	DATA data;
	struct Node_t *prev;
} NODE;

/* the HEAD of the Queue, hold the amount of node's that are in the queue*/
typedef struct Queue {
	NODE *head;
	NODE *tail;
	int size;
	int limit;
} Queue;

Queue *ConstructQueue(int limit);
void DestructQueue(Queue *queue);
int Enqueue(Queue *pQueue, NODE *item);
NODE *Dequeue(Queue *pQueue);
int isEmpty(Queue* pQueue);