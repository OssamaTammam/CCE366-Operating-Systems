# Lab 4 - Multithreading
Implemented two types of matrix multiplication algorithm and merge sort algorithms using multithreading for better performance that using a single heavyweight thread (process).


# Matrix Multiplication
## Data Structures
* Matrix data structure instead of just using 2D arrays
    ```c
    typedef struct
    {
        int rows;
        int cols;
        int **data; 
    } Matrix;
    ```
* ThreadArgs data structure to pass on arguments to the thread functions
    ```c
    typedef struct
    {
        Matrix *matrixA;
        Matrix *matrixB;
        Matrix *matrixResult;
        int currRow;
        int currCol;
    } ThreadArgs;
    ```

## Program Flow
* Program takes the input .txt file name containing the test case on the form and dynamically allocating memory to them
    ```
    [number of rows of 1st matrix] [number of columns of 1st matrix]
    1st matrix entries
    [number of rows of 2nd matrix] [number of columns of 2nd matrix]
    2nd matrix entries
    ```
* The program then feeds that input into the ```matrixMul``` function that then based on the ```elementRowFlag``` decides the way we are gonna do our multithreading which be either multithreading each element or multithreading each row
* The program then starts multithreading and waits for the threads to join then prints the output matrix including the time it took to compare between element multithreading and row multithreading on the form
    ```
    [result matrix entries]
    END1 [elapsed time of procedure 1]
    [result matrix entries]
    END2 [elapsed time of procedure 2]
    ```

# Merge Sort
## Overview
Given the divide and conquer nature of merge sort implementing recursive multithreaded calls with pointers was tricky, but using pointers it can be done without using a lot of memory.

## Data structures
* ThreadArgs data structure to pass on arguments to the thread functions
    ```c
    typedef struct
    {
        int **arr;
        int low;
        int high
    } ThreadArgs;
    ```

## Program Flow
* Program takes the input .txt file name containing the test case on the form and dynamically allocating memory to them
    ```c
    [number of elements]
    Array elements that are space-separated
    ```
* Program then calls the ```mergeSort``` function which starts our multithreaded merge sort algorithm using ```_mergeSort``` helper function
    ### Merge Sort Algorithm
    * Base Case: if the ```low>=high``` that means we successfully recursed and divided the entire array and we should start the conquer step
    * The algorithm splits the array into two equally sized arrays and then calls itself onto them until it hits the base case
    * Once every recursive call is done we wait on our threads to join so we can start merging using hte ```merge``` function
    * Merge function takes two arrays sorts them using comparisons and stores them into the original array
*  Every recursive call is done on a thread in parallel with the other threads to increase performance
