#include "queue.h"
#include <stdlib.h>

static const QueueData ZeroData = {0};

Queue *queue_new(int capacity) {
    Queue *queue = malloc(sizeof(Queue));
    queue->capacity = capacity;
    queue->data = malloc(sizeof(QueueData) * capacity);
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

int queue_size(Queue *queue) {
    return abs((queue->rear - queue->front) % queue->capacity);
}

int queue_enqueue(Queue *queue, QueueData data) {
    if (queue_is_full(queue)) {
        return 0;
    }

    queue->data[queue->rear] = data;
    queue->rear = (queue->rear + 1) % queue->capacity;
    return 1;
}

int queue_enqueue_char(Queue *queue, char c) {
  QueueData data = ZeroData;
  data.charValue = c;
  return queue_enqueue(queue, data);
}

int queue_enqueue_int(Queue *queue, int i) {
  QueueData data = ZeroData;
  data.intValue = i;
  return queue_enqueue(queue, data);
}

int queue_enqueue_ptr(Queue *queue, void *ptr) {
  QueueData data = ZeroData;
  data.ptrValue = ptr;
  return queue_enqueue(queue, data);
}

QueueData queue_dequeue(Queue *queue) {
    if (queue_is_empty(queue)) {
        return ZeroData;
    }

    QueueData data = queue->data[queue->front];
    queue->front = (queue->front + 1) % queue->capacity;
    return data;
}

char queue_dequeue_char(Queue *queue) {
  return queue_dequeue(queue).charValue;
}

int queue_dequeue_int(Queue *queue) {
  return queue_dequeue(queue).intValue;
}

void* queue_dequeue_ptr(Queue *queue) {
  return queue_dequeue(queue).ptrValue;
}

int queue_peek(Queue *queue, QueueData *data) {
    if (queue_is_empty(queue)) {
        return 0;
    }

    if (data) {
        *data = queue->data[queue->front];
    }
    
    return 1;
}

int queue_peek_ptr(Queue *queue, void **ptr) {
  QueueData data = ZeroData;
  int result = queue_peek(queue, &data);
  if (ptr) *ptr = data.ptrValue;
  return result;
}

int queue_peeki(Queue *queue, int offset, QueueData *data) {
    if (offset >= queue_size(queue)) {
        return 0;
    }

    if (data) {
        *data = queue->data[(queue->front + offset) % queue->capacity];
    }
    
    return 1;
}

int queue_peeki_char(Queue *queue, int offset, char *c) {
  QueueData data = ZeroData;
  int result = queue_peeki(queue, offset, &data);
  if (c) *c = data.charValue;
  return result;
}
