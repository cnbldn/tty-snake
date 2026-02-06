#include "coordQueue.h"

void queueInit(Queue *queue) {
    queue->front = queue->rear = NULL;
    queue->size = 0;
}

void enqueue(Queue *queue, coord data) {
    struct Node *newnode = malloc(sizeof(struct Node));
    newnode->data = data;
    newnode->next = NULL;

    if (queue->size == 0) {
        queue->front = queue->rear = newnode;
        queue->size++;
        return;
    }

    queue->rear->next = newnode;
    queue->rear = newnode;
    queue->size++;
}

void dequeue(Queue *queue, coord *ret) {
    if (queue->size == 0) {
        ret = NULL;
        return;
    }
    *ret = queue->front->data;
    struct Node *tempFront = queue->front;
    queue->front = queue->front->next;
    free(tempFront);
    queue->size--;
}
