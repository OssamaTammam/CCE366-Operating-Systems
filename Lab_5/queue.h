// queue.h
#ifndef QUEUE_H
#define QUEUE_H

// Node for the queue
typedef struct Node
{
    int data;
    int index;
    struct Node *next;
} Node;

// Queue
typedef struct
{
    Node *front;
    Node *rear;
} Queue;

// Function declarations
void initializeQueue(Queue *queue);
int isEmpty(Queue *queue);
void enqueue(Queue *queue, int data);
int dequeue(Queue *queue);
int getSize(Queue *queue);

#endif // QUEUE_H
