#ifndef __QUEUE_H__
#define __QUEUE_H__

typedef struct {
    int capacity;
    void **data;
    int front;
    int rear;
} Queue;

Queue *queue_new(int capacity);
int queue_is_empty(Queue *queue);
int queue_is_full(Queue *queue);
int queue_size(Queue *queue);
int queue_enqueue(Queue *queue, void *data);
void *queue_dequeue(Queue *queue);
int queue_peek(Queue *queue, void **data);
int queue_peeki(Queue *queue, int offset, void **data);

#endif
