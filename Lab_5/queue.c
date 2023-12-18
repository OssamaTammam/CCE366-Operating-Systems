#include <stdio.h>
#include <stdlib.h>

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

// Function to initialize an empty queue
void initializeQueue(Queue *queue)
{
    queue->front = NULL;
    queue->rear = NULL;
}

// Function to check if the queue is empty
int isEmpty(Queue *queue)
{
    return (queue->front == NULL);
}

// Function to enqueue a new element
void enqueue(Queue *queue, int data)
{
    Node *newNode = (Node *)malloc(sizeof(Node));
    if (newNode == NULL)
    {
        fprintf(stderr, "Memory allocation error\n");
        exit(EXIT_FAILURE);
    }

    newNode->data = data;
    newNode->next = NULL;

    if (isEmpty(queue))
    {
        queue->front = newNode;
        queue->rear = newNode;
        newNode->index = 0;
    }
    else
    {
        newNode->index = (queue->rear->index) + 1;
        queue->rear->next = newNode;
        queue->rear = newNode;
    }
}

// Function to dequeue an element
int dequeue(Queue *queue)
{
    if (isEmpty(queue))
    {
        fprintf(stderr, "Queue is empty\n");
        exit(EXIT_FAILURE);
    }

    Node *temp = queue->front;
    int data = temp->data;

    if (queue->front == queue->rear)
    {
        queue->front = NULL;
        queue->rear = NULL;
    }
    else
    {
        queue->front = queue->front->next;
        Node *currNode = queue->front;
        while (currNode != NULL)
        {
            (currNode->index)--;
            currNode = currNode->next;
        }
    }

    free(temp);
    return data;
}
