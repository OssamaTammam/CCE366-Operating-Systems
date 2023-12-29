#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>
#include "queue.h"

#define N_M_COUNTER 10
#define BUFFER_SIZE 10
#define SLEEP_TIME 0

sem_t counterMutex, bufferFull, bufferEmpty, bufferMutex;

typedef struct
{
    int threadIndex;
    int *messageCount;
} CounterThreadArgs;

typedef struct
{
    int *messageCount;
    Queue *buffer;
} MonitorThreadArgs;

void *
sendMessages(void *args)
{
    int index = ((CounterThreadArgs *)args)->threadIndex;
    int *messageCount = ((CounterThreadArgs *)args)->messageCount;
    int randomTime;

    // Make the thread ongoing so it just sleeps for a random time
    while (1)
    {
        // Sleep for a random amount of time between 1 and 20 seconds
        randomTime = 1 + (rand() % 1);

        sleep(randomTime);

        printf("Counter Thread %d: Received a message!\n", index);
        printf("Counter Thread %d: Waiting to write\n", index);

        // Wait until the thread is the only one accessing the counter
        sem_wait(&counterMutex);

        // Critical part of the thread function
        // Add message to the number of messages received
        printf("Counter Thread %d: Now adding to counter, counter value is %d\n", index, ((*messageCount) + 1));
        (*messageCount)++;
        sleep(SLEEP_TIME);

        // Signal the semaphore this thread is done accessing the counter
        sem_post(&counterMutex);
    }
}

void *monitorMessages(void *args)
{
    Queue *buffer = ((MonitorThreadArgs *)args)->buffer;
    int *messageCount = ((MonitorThreadArgs *)args)->messageCount;
    int randomTime, semValue;

    // Make the thread ongoing so it just sleeps for a random time
    while (1)
    {
        // Sleep for a random amount of time between 1 and 20 seconds
        randomTime = 1 + (rand() % 2);
        sleep(randomTime);

        // Wait until the thread is the only one accessing the counter
        printf("Monitor Thread: Waiting to read counter\n");
        sem_wait(&counterMutex);

        // Critical part of the thread
        // Place the number of messages till now in the buffer
        int producerBuffer = *messageCount;
        *messageCount = 0;
        printf("Monitor Thread: Reading a count value of %d\n", producerBuffer);

        // Signal the semaphore this thread is done accessing the counter
        sem_post(&counterMutex);

        // Producer part of the problem
        // Check if our buffer size can accept new additions
        if (sem_getvalue(&bufferEmpty, &semValue) > -1)
        {
            int noElements = getSize(buffer);
            printf("-----------this is my sem value {%d} - noELement: {%d}\n", semValue, noElements);
        }

        if (semValue == BUFFER_SIZE)
        {
            printf("Monitor Thread: BUFFER FULL!!\n");
        }
        sleep(SLEEP_TIME);

        sem_wait(&bufferEmpty);
        sem_wait(&bufferMutex);

        // Enqueue our message count into out buffer queue
        printf("Monitor Thread: Writing to buffer at index %d\n", buffer->rear ? ((buffer->rear->index) + 1) : 0);
        enqueue(buffer, producerBuffer);
        sleep(SLEEP_TIME);

        // Signal the semaphores
        sem_post(&bufferMutex);
        sem_post(&bufferFull);
    }
}

void *collectMessages(void *buffer)
{
    buffer = (Queue *)buffer;
    int randomTime;

    while (1)
    {
        // Sleep for a random amount of time between 1 and 20 seconds
        randomTime = 1 + (rand() % 5);
        sleep(randomTime);

        printf("Collector Thread: Waiting to read from buffer\n");
        sem_wait(&bufferFull);
        sem_wait(&bufferMutex);

        // Dequeue our latest count
        printf("Collector Thread: Reading from buffer a count of %d\n", dequeue(buffer));
        sleep(SLEEP_TIME);

        // Signal the semaphores
        sem_post(&bufferMutex);
        sem_post(&bufferEmpty);
    }
}

int main()
{
    // Random seed with current time
    srand(time(NULL));

    // Initialize used semaphores
    sem_init(&counterMutex, 0, 1);
    sem_init(&bufferFull, 0, 0);
    sem_init(&bufferEmpty, 0, BUFFER_SIZE);
    sem_init(&bufferMutex, 0, 1);

    // Resources used
    int *messageCount = malloc(sizeof(int));
    Queue *buffer = malloc(sizeof(Queue));
    *messageCount = 0;
    initializeQueue(buffer);

    // Threads used
    pthread_t mCounter[N_M_COUNTER],
        mMonitor, mCollector;

    // Launching our threads
    // Threads sending the messages
    CounterThreadArgs **counterArgs;
    counterArgs = malloc(sizeof(CounterThreadArgs *) * N_M_COUNTER);
    for (int i = 0; i < N_M_COUNTER; i++)
    {
        counterArgs[i] = malloc(sizeof(CounterThreadArgs));
        counterArgs[i]->threadIndex = i;
        counterArgs[i]->messageCount = messageCount;
        pthread_create(&mCounter[i], NULL, sendMessages, counterArgs[i]);
    }

    // Thread monitoring our message count and placing into the buffer ~ Producer part
    MonitorThreadArgs *producerArgs = malloc(sizeof(MonitorThreadArgs));
    producerArgs->buffer = buffer;
    producerArgs->messageCount = messageCount;
    pthread_create(&mMonitor, NULL, monitorMessages, (void *)producerArgs);

    // Thread collecting from our buffer ~ Consumer part
    pthread_create(&mCollector, NULL, collectMessages, buffer);

    // Join Threads
    for (int i = 0; i < N_M_COUNTER; i++)
    {
        pthread_join(mCounter[i], NULL);
    }
    pthread_join(mMonitor, NULL);
    pthread_join(mCollector, NULL);

    // Free allocated memory
    sem_destroy(&counterMutex);
    sem_destroy(&bufferEmpty);
    sem_destroy(&bufferFull);
    sem_destroy(&bufferMutex);
    free(messageCount);
    free(buffer);
    free(producerArgs);
    free(counterArgs);

    return 0;
}