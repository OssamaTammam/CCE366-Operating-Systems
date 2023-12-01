#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

typedef struct
{
    int rows;
    int cols;
    int **data; // Double pointer so each pointer points to a pointer that points to an array
} Matrix;

typedef struct
{
    Matrix *matrixA;
    Matrix *matrixB;
    Matrix *matrixResult;
    int currRow;
    int currCol;
} ThreadArgs;

// Read the matrices from the file and return them
void readMatrices(Matrix *matrixA, Matrix *matrixB, int testCase)
{
    char fileName[30]; // String to store file name assuming test cases don't go past 3 digits
    sprintf(fileName, "test_cases/test-case-%d.txt", testCase);
    FILE *matricesFile = fopen(fileName, "r");

    // Error handling in case file doesn't exist or something
    if (matricesFile == NULL)
    {
        printf("Error opening file: %s\n", fileName);
        exit(EXIT_FAILURE); // Terminate with an error status
    }

    // Allocate space in memory for the data
    int rowsA, colsA;
    fscanf(matricesFile, "%d %d", &rowsA, &colsA);
    matrixA->rows = rowsA;
    matrixA->cols = colsA;
    matrixA->data = (int **)malloc(rowsA * sizeof(int *));
    for (int i = 0; i < rowsA; ++i)
    {
        matrixA->data[i] = (int *)malloc(colsA * sizeof(int));
    }

    // Read matrix elements from file
    for (int i = 0; i < matrixA->rows; i++)
    {
        for (int j = 0; j < matrixA->cols; j++)
        {
            fscanf(matricesFile, "%d", &matrixA->data[i][j]);
        }
    }

    int rowsB, colsB;
    fscanf(matricesFile, "%d %d", &rowsB, &colsB);
    matrixB->rows = rowsB;
    matrixB->cols = colsB;
    matrixB->data = (int **)malloc(rowsB * sizeof(int *));
    for (int i = 0; i < rowsB; ++i)
    {
        matrixB->data[i] = (int *)malloc(colsB * sizeof(int));
    }

    // Read matrix elements from file
    for (int i = 0; i < matrixB->rows; i++)
    {
        for (int j = 0; j < matrixB->cols; j++)
        {
            fscanf(matricesFile, "%d", &matrixB->data[i][j]);
        }
    }

    fclose(matricesFile);
}

void printMatrix(Matrix *matrix)
{
    printf("No of rows: %d\nNo of columns: %d\n", matrix->rows, matrix->cols);
    for (int i = 0; i < matrix->rows; i++)
    {
        for (int j = 0; j < matrix->cols; j++)
        {
            if (matrix->data[i][j] >= 0)
                printf(" %d\t", matrix->data[i][j]);
            else
                printf("%d\t", matrix->data[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

void *computeElement(void *arg)
{
    // Get our arguments from the void pointer
    ThreadArgs *threadArgs = (ThreadArgs *)arg;

    // Start computing the element
    int ans = 0;
    for (int i = 0; i < threadArgs->matrixA->cols; i++)
    {
        ans += threadArgs->matrixA->data[threadArgs->currRow][i] * threadArgs->matrixB->data[i][threadArgs->currCol];
    }

    threadArgs->matrixResult->data[threadArgs->currRow][threadArgs->currCol] = ans;

    return NULL;
}

void elementMatrixMul(Matrix *matrixA, Matrix *matrixB, Matrix *matrixResult)
{
    // Start threading the multiplications
    int numThreads = matrixResult->rows * matrixResult->cols;
    pthread_t threads[numThreads]; // Array to store all threads

    ThreadArgs currThreadArgs[numThreads];
    int currThread = 0;
    for (int row = 0; row < matrixResult->rows; row++)
    {
        for (int col = 0; col < matrixResult->cols; col++)
        {
            currThreadArgs[currThread].matrixA = matrixA;
            currThreadArgs[currThread].matrixB = matrixB;
            currThreadArgs[currThread].matrixResult = matrixResult;
            currThreadArgs[currThread].currRow = row;
            currThreadArgs[currThread].currCol = col;

            pthread_create(&threads[currThread], NULL, computeElement, (void *)&currThreadArgs[currThread]);
            currThread++;
        }
    }

    // Wait for all threads to finish
    for (int i = 0; i < numThreads; i++)
    {
        pthread_join(threads[i], NULL);
    }
}

void *computeRow(void *arg)
{
    // Get our arguments from the void pointer
    ThreadArgs *threadArgs = (ThreadArgs *)arg;
    int rowResult = threadArgs->currRow;

    // Start computing the element
    for (int colResult = 0; colResult < threadArgs->matrixB->cols; colResult++)
    {
        int ans = 0;
        for (int i = 0; i < threadArgs->matrixA->cols; i++)
        {
            ans += threadArgs->matrixA->data[threadArgs->currRow][i] * threadArgs->matrixB->data[i][colResult];
        }

        threadArgs->matrixResult->data[rowResult][colResult] = ans;
    }

    return NULL;
}

void rowMatrixMul(Matrix *matrixA, Matrix *matrixB, Matrix *matrixResult)
{
    // Start threading the multiplications
    int numThreads = matrixResult->rows;
    pthread_t threads[numThreads]; // Array to store all threads

    ThreadArgs currThreadArgs[numThreads];
    int currThread = 0;
    for (int row = 0; row < matrixResult->rows; row++)
    {
        currThreadArgs[currThread].matrixA = matrixA;
        currThreadArgs[currThread].matrixB = matrixB;
        currThreadArgs[currThread].matrixResult = matrixResult;
        currThreadArgs[currThread].currRow = row;

        pthread_create(&threads[currThread], NULL, computeRow, (void *)&currThreadArgs[currThread]);
        currThread++;
    }

    // Wait for all threads to finish
    for (int i = 0; i < numThreads; i++)
    {
        pthread_join(threads[i], NULL);
    }
}

void matrixMul(Matrix *matrixA, Matrix *matrixB, Matrix *matrixResult, int elementRowFlag)
{
    // Check if matrices can be multiplied first
    if (matrixA->cols != matrixB->rows)
    {
        printf("Can't multiply! Dimension are not right\n");
        exit(EXIT_FAILURE);
    }

    // Matrices can be multiplied
    // Allocate space for the resulting matrix
    matrixResult->rows = matrixA->rows;
    matrixResult->cols = matrixB->cols;

    matrixResult->data = (int **)malloc(matrixResult->rows * sizeof(int *));
    for (int i = 0; i < matrixResult->rows; i++)
    {
        matrixResult->data[i] = (int *)malloc(matrixResult->cols * sizeof(int));
    }

    // 0 -> element threading, 1 -> row threading
    elementRowFlag ? rowMatrixMul(matrixA, matrixB, matrixResult) : elementMatrixMul(matrixA, matrixB, matrixResult);
}

int main()
{
    // Read the matrices from a file
    Matrix matrixA, matrixB;
    int testCase = 1; // To specify which file
    readMatrices(&matrixA, &matrixB, testCase);

    // Print matrices to check if read is correct
    printf("Matrix 1:\n");
    printMatrix(&matrixA);
    printf("Matrix 2:\n");
    printMatrix(&matrixB);

    // Computation of each element of the output matrix happens in a thread
    double elementTime;
    clock_t start, end;

    Matrix matrixResultElement;

    start = clock();
    matrixMul(&matrixA, &matrixB, &matrixResultElement, 0);
    end = clock();

    elementTime = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("Matrix Multiplication Result:\n");
    printMatrix(&matrixResultElement);
    printf("END1\tTime took: %f\n\n", elementTime);

    // Computation of each row of the output matrix happens in a thread
    Matrix matrixResultRow;

    start = clock();
    matrixMul(&matrixA, &matrixB, &matrixResultRow, 1);
    end = clock();

    elementTime = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("Matrix Multiplication Result:\n");
    printMatrix(&matrixResultRow);
    printf("END2\tTime took: %f\n\n", elementTime);

    return 0;
}