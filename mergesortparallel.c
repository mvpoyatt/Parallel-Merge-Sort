/*
 * File: mergesortparallel.c
 * Author: Katie Levy and Michael Poyatt
 * Date: 10/21/16
 * Description: Parallel mergesort algorithm
 */

#include<pthread.h>
#include"mergesortparallel.h"

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

// MergeSort a global array in parallel.
int mergeSortParallel() {
    threads = malloc(threadCount*sizeof(pthread_t));
    int i;
    for(i = 0; i < threadCount; i++){
        long rank = (long)i;
        pthread_create(&threads[i], NULL, threadFunc, (void*)rank);
    }
    for(i = 0; i < threadCount; i++){
        pthread_join(threads[i], NULL);
    }
    return 0;
}

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

    printf("Range: %ld -- %ld\n", myFirsti, myLasti);
    driverParallel(myFirsti, myLasti-1);
    
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

void printParallel(int start, int stop){  
    int i;
    for(i = start; i < stop; i++){
        printf("%d\t", vecParallel[i]);
    }
    printf("\n");
    return;
}
