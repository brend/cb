#ifndef __QUEUE_H__
#define __QUEUE_H__

typedef union {
	char charValue;
	int intValue;
	void *ptrValue;
} QueueData;

typedef struct {
    int capacity;
    QueueData *data;
    int front;
    int rear;
} Queue;

Queue *queue_new(int capacity);

int queue_is_empty(Queue *queue);
int queue_is_full(Queue *queue);
int queue_size(Queue *queue);

int queue_enqueue(Queue *queue, QueueData data);
int queue_enqueue_char(Queue *, char c);
int queue_enqueue_int(Queue *, int i);
int queue_enqueue_ptr(Queue *, void *ptr);

QueueData queue_dequeue(Queue *queue);
char queue_dequeue_char(Queue *queue);
int queue_dequeue_int(Queue *queue);
void *queue_dequeue_ptr(Queue *queue);

int queue_peek(Queue *queue, QueueData *data);
int queue_peeki(Queue *queue, int offset, QueueData *data);

#endif
