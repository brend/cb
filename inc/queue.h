#ifndef __QUEUE_H__
#define __QUEUE_H__

typedef struct {
    int capacity;
    int *data;
    int front;
    int rear;
} Queue;

Queue *queue_new(int capacity);
int queue_is_empty(Queue *queue);
int queue_is_full(Queue *queue);
int queue_enqueue(Queue *queue, int data);
int queue_dequeue(Queue *queue);
int queue_peek(Queue *queue, int *data);

#endif