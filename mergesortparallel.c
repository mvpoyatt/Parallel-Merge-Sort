/*
 * File: mergesortparallel.c
 * Author: Katie Levy and Michael Poyatt
 * Date: 10/21/16
 * Description: Parallel mergesort algorithm
 */

#include<pthread.h>
#include"mergesortparallel.h"

/* Function Declarations */
int threadFunc(void* rank);
int validateParallel();

/* Global variables */
extern int threadCount;
extern long n; //array size
extern int *vecSerial;
extern int *vecParallel;
extern int *temp;
pthread_t *threads;

// MergeSort a global array in parallel.
int mergeSortParallel() {
    threads = malloc(threadCount*sizeof(pthread_t));
    int i;
    for(i = 0; i < threadCount; i++){
        pthread_create(&threads[i], NULL, threadFunc, (void *)i);
    }
    for(i = 0; i < threadCount; i++){
        pthread_join(threads[i], NULL);
    }
    return 0;
}

int threadFunc(void* rank){
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

    printf("Hello from thread %ld\n", myRank);
    
    // Serial mergesort portion and store in global partial sort array
    // TODO: change serial mergesort to noncontiguous subarrays 

    // Tree-based parallel reduction to merge sorted subarrays
    // TODO: write reduction

    return 0;
}

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
