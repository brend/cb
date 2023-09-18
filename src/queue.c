#include "queue.h"
#include <stdlib.h>

Queue *queue_new(int capacity) {
    Queue *queue = malloc(sizeof(Queue));
    queue->capacity = capacity;
    queue->data = malloc(sizeof(int) * capacity);
    queue->front = 0;
    queue->rear = 0;
    return queue;
}

int queue_is_empty(Queue *queue) {
    return queue->front == queue->rear;
}

int queue_is_full(Queue *queue) {
    return queue->front == (queue->rear + 1) % queue->capacity;
}

int queue_enqueue(Queue *queue, int data) {
    if (queue_is_full(queue)) {
        return 0;
    }

    queue->data[queue->rear] = data;
    queue->rear = (queue->rear + 1) % queue->capacity;
    return 1;
}

int queue_dequeue(Queue *queue) {
    if (queue_is_empty(queue)) {
        return 0;
    }

    int data = queue->data[queue->front];
    queue->front = (queue->front + 1) % queue->capacity;
    return data;
}

int queue_peek(Queue *queue, int *data) {
    if (queue_is_empty(queue)) {
        return 0;
    }

    if (data) {
        *data = queue->data[queue->front];
    }
    
    return 1;
}