#ifndef QUEUE_H
#define QUEUE_H

#include "globals.h"
#include <stdlib.h>

struct Node {
    coord data;
    struct Node *next;
};

typedef struct {
    int size;
    struct Node *front;
    struct Node *rear;
} Queue;

void queueInit(Queue *queue);
void enqueue(Queue *queue, coord data);
void dequeue(Queue *queue, coord *ret);
#endif // !QUEUE_H
