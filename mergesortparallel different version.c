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
int binSearch(int arr[], int a, int b, int x);
void multiMerge(int numThreads, Block first, Block second, long tempStart, long tempEnd, long my_rank);


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
        if(threads_ready == threadCount){
            for(int i = 0; i < threads_ready; i++){
                pthread_cond_signal(&ready_cv);
            }
            threads_ready = 0;
        }else {
            pthread_cond_wait(&ready_cv, &lock);
        }
        pthread_mutex_unlock(&lock);

        Block beginningRecur;
        Block beginningRecur2;
        if(myRank % divisor == 0){
            partner = myRank + difference;
            if(partner < threadCount){
                beginningRecur.start = myFirsti;
                beginningRecur.end = myLasti;
                beginningRecur2.start = myLasti + 1;
                beginningRecur2.end = myFirsti + (pow(2, difference) * (n / threadCount));
            }
        } else {

        }
        multiMerge(divisor, beginningRecur, beginningRecur2, 0, n, myRank);
        divisor *= 2;
        difference *=2;
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

// Recursive binary search
int binSearch(int arr[], int a, int b, int x){
    if (b >= a){
        int mid = a + (b - a)/2;
        if (arr[mid] == x){
            return mid;}
        if(arr[mid] > x){
            return binSearch(arr, a, mid-1, x);}
        return binSearch(arr, mid+1, b, x);
    }
    return a;
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
void multiMerge(int numThreads, Block first, Block second, long tempStart, long tempEnd, long my_rank){
    // A barrier for all threads to be on the same level
        pthread_mutex_lock(&lock);
        threads_ready++;
        if(threads_ready == threadCount){
            for(int i = 0; i < threads_ready; i++){
                pthread_cond_signal(&ready_cv);
            }
            threads_ready = 0;
        }else {
            pthread_cond_wait(&ready_cv, &lock);
        }
        pthread_mutex_unlock(&lock);
    if (numThreads == 1){
        printf("Thread %ld merging first %ld - %ld and second %ld - %ld", my_rank, first.start, first.end, second.start, second.end);
    } else {
        long x1start, x2start, x1end, x2end, xsize;
        long y1start, y2start, y1end, y2end, y1size, y2size;
        x1start = first.start;
        x2start = (first.start + first.end + 1) / 2;
        x1end = x2start;
        x2end = first.end;
        xsize = x1end - x1start;

        y1start = second.start;
        y2start = binSearch(vecParallel, second.start, second.end, vecParallel[x2start]);
        y1end = y2start;
        y2end = second.end;
        y1size = y1end - y1start;
        y2size = y2end - y2start;

        numThreads /= 2;
        if(my_rank % numThreads < numThreads / 2){
            Block fir;
            fir.start = x1start;
            fir.end = x1end;
            Block sec;
            sec.start = y1start;
            sec.end = y1end;
            tempStart = x1start;
            tempEnd = x1end + y1size;
            printf("Thread %ld first: %ld --> %ld\n", my_rank, fir.start, fir.end);
            printf("Thread %ld second: %ld --> %ld\n", my_rank, sec.start, sec.end);
            multiMerge(numThreads, fir, sec, tempStart, tempEnd, my_rank);
        } else {
            Block fir; 
            fir.start = x2start;
            fir.end = x2end;
            Block sec;
            sec.start = y2start;
            sec.end = y2end;
            tempStart = x1end + y1size;
            tempEnd = tempStart + xsize + y2size;
            printf("Thread %ld first: %ld --> %ld\n", my_rank, fir.start, fir.end);
            printf("Thread %ld second: %ld --> %ld\n", my_rank, sec.start, sec.end);
            multiMerge(numThreads, fir, sec, tempStart, tempEnd, my_rank);
        }
    }
    return;
}
