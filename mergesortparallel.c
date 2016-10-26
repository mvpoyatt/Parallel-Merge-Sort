/*
 * File: mergesortparallel.c
 * Author: Katie Levy and Michael Poyatt
 * Date: 10/25/16
 * Description: Parallel mergesort algorithm
 */

#include <pthread.h>
#include <math.h>
#include "mergesortparallel.h"


/* Function Declarations */
void* threadFunc(void* rank);
int driverParallel(int start, int stop);
int mergeParallel(int start, int middle, int stop);
void printParallel(int start, int stop);
int validateParallel();

/* Global variables */
extern int threadCount;
extern long n; //array size
extern int *vecSerial;
extern int *vecParallel;
extern int *temp;
pthread_t *threads;
extern int threads_ready;
extern pthread_cond_t ready_cv;
extern pthread_mutex_t lock;

// MergeSort a global array in parallel.
int mergeSortParallel() {
    // Allocate memory for threads
    threads = malloc(threadCount*sizeof(pthread_t));
    // Create threads
    int i;
    for(i = 0; i < threadCount; i++){
        long rank = (long)i;
        pthread_create(&threads[i], NULL, threadFunc, (void*)rank);
    }
    // Join/terminate all the threads
    for(i = 0; i < threadCount; i++){
        pthread_join(threads[i], NULL);
    }
    return 0;
}

// Function for each thread to perform.
// It mergesorts a portion of the array and mergesorts the rest using 
// a tree-reduction algorithm.
void* threadFunc(void* rank){
     // Get rank of this thread.
    long myRank = (long) rank;  /* Use long in case of 64-bit system */
    
    // Compute starting and ending indices for this thread to sum.
    long quotient = n / threadCount;
    long remainder = n % threadCount;
    long myCount;
    long myFirsti, myLasti;
    if (myRank < remainder) {
        myCount = quotient + 1;
        myFirsti = myRank * myCount;
    }
    else {
        myCount = quotient;
        myFirsti = myRank * myCount + remainder;
    }
    myLasti = myFirsti + myCount;
    
    // Mergesort that section of the array
    driverParallel(myFirsti, myLasti-1);

    int divisor = 2;
    int difference = 1;
    int partner = 0;

    // Tree reduction to divide work amoung threads
    while(difference < threadCount) {
        // A barrier for all threads to be on the same level
        pthread_mutex_lock(&lock);
        threads_ready++;
        if(threads_ready == threadCount / difference){
            for(int i = 0; i < threads_ready; i++){
                pthread_cond_signal(&ready_cv);
            }
            threads_ready = 0;
        }else {
            pthread_cond_wait(&ready_cv, &lock);
        }
        pthread_mutex_unlock(&lock);
        // Thread doing the work
        if(myRank % divisor == 0){
            partner = myRank + difference;
            if(partner < threadCount){
                myLasti = myFirsti + (pow(2, difference) * (n / threadCount));
                driverParallel(myFirsti, myLasti-1);
            }
        // Thread not doing work
        } else {
            break;
        }
        divisor *= 2;
        difference *= 2;
    }
    return 0;
}

// Validates the sorting of the parallel algorithm
int validateParallel(){
    int i;
    for(i = 0; i < n-1; i++){
        if(vecParallel[i] != vecSerial[i]){
            printf("Parallel sort unsuccesful.\n");
            return 1;
        }
    }
    return 0;
}

// Recursively divides the array and merges
int driverParallel(int start, int stop){
    if(start >= stop){
        return 0;
    }
    int middle = ((stop + start) / 2);
    driverParallel(start, middle);
    driverParallel(middle+1, stop);
    mergeParallel(start, middle, stop);
    return 0;
}

// Merges two arrays
int mergeParallel(int start, int middle, int stop){
    int first = start;
    int second = middle+1;
    int tempIndex = start;
    while(first <= middle && second <= stop){
        if(vecParallel[first] < vecParallel[second]){
            temp[tempIndex] = vecParallel[first];
            first++;
            tempIndex++;
        } else {
            temp[tempIndex] = vecParallel[second];
            second++;
            tempIndex++;
        }
    }
    while(first <= middle){
        temp[tempIndex] = vecParallel[first];
            first++;
            tempIndex++;
    }
    while(second <= stop){
        temp[tempIndex] = vecParallel[second];
            second++;
            tempIndex++;
    }
    int i;
    for(i = start; i <= stop; i++){
        vecParallel[i] = temp[i];
    }
    return 0;
}

// Prints the array used in parallel mergesort
void printParallel(int start, int stop){  
    int i;
    for(i = start; i < stop; i++){
        printf("%d\t", vecParallel[i]);
    }
    printf("\n");
    return;
}
