#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

typedef struct
{
    int **arr;
    int low;
    int high
} ThreadArgs;

int readArray(int **arr, char *fileName)
{
    FILE *arrayFile = fopen(fileName, "r");

    if (arrayFile == NULL)
    {
        printf("Error opening file: %s\n", fileName);
        exit(EXIT_FAILURE); // Terminate with an error status
    }

    int noElement;
    fscanf(arrayFile, "%d", &noElement);
    *arr = (int *)malloc(noElement * sizeof(int));

    for (int i = 0; i < noElement; i++)
    {
        fscanf(arrayFile, "%d", &(*arr)[i]);
    }

    fclose(arrayFile);

    return noElement;
}

void printArray(int **arr, int noElements)
{
    for (int i = 0; i < noElements; i++)
    {
        printf("%d ", (*arr)[i]);
    }
    printf("\n");
}

void merge(int **arr, int low, int high, int middle)
{
    // Init helper arrays
    int noElementsLeft = middle - low + 1;
    int noElementRight = high - middle;
    int leftArray[noElementsLeft], rightArray[noElementRight];

    // Copy elements of original array into them
    for (int i = 0; i < noElementsLeft; i++)
    {
        leftArray[i] = (*arr)[low + i];
    }
    for (int j = 0; j < noElementRight; j++)
    {
        rightArray[j] = (*arr)[middle + 1 + j];
    }

    // Merge the arrays
    int i = 0;
    int j = 0;
    int k = low;

    while (i < noElementsLeft && j < noElementRight)
    {
        if (leftArray[i] < rightArray[j])
        {
            (*arr)[k++] = leftArray[i++];
        }
        else
        {
            (*arr)[k++] = rightArray[j++];
        }
    }

    // Check for any remaining elements
    while (i < noElementsLeft)
    {
        (*arr)[k++] = leftArray[i++];
    }
    while (j < noElementRight)
    {
        (*arr)[k++] = rightArray[j++];
    }
}

void *_mergeSort(void *arg)
{
    ThreadArgs *args = (ThreadArgs *)arg;

    if (args->low >= args->high)
        return;

    // Calculate the middle of the current divided array
    int middle = (args->high + args->low) / 2;

    // Args for divide step
    ThreadArgs argsLeft, argsRight;
    argsLeft.arr = args->arr;
    argsRight.arr = args->arr;
    argsLeft.low = args->low;
    argsLeft.high = middle;
    argsRight.low = middle + 1;
    argsRight.high = args->high;

    // Threads used
    pthread_t threadLeft, threadRight;

    // Divide step
    pthread_create(&threadLeft, NULL, _mergeSort, (void *)&argsLeft);
    pthread_create(&threadRight, NULL, _mergeSort, (void *)&argsRight);

    // Wait for threads to join
    pthread_join(threadLeft, NULL);
    pthread_join(threadRight, NULL);

    // Conquer step
    merge(args->arr, args->low, args->high, middle);
}

void mergeSort(int **arr, int noElements)
{
    ThreadArgs args;
    args.low = 0;
    args.high = noElements - 1;
    args.arr = arr;
    pthread_t startingThread;

    // Start the merge sort algorithm
    pthread_create(&startingThread, NULL, _mergeSort, (void *)&args);
    pthread_join(startingThread, NULL);
}

int main()
{
    char *fileName = malloc(100);
    printf("Please enter file name: ");
    scanf("%99s", fileName);

    int *arr;
    int noElements = readArray(&arr, fileName);
    printArray(&arr, noElements);

    free(fileName);

    mergeSort(&arr, noElements);

    printf("Sorted array:\n");
    printArray(&arr, noElements);
    return 0;
}