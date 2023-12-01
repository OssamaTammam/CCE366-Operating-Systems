#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

typedef struct
{
    int rows;
    int cols;
    int **data; // Double pointer so each pointer points to a pointer that points to an array
} Matrix;

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
}

int main()
{
    // Read the matrices from a file
    Matrix matrixA, matrixB;
    int testCase = 1; // To specify which file
    readMatrices(&matrixA, &matrixB, testCase);
    printMatrix(&matrixA);
    printMatrix(&matrixB);
    return 0;
}