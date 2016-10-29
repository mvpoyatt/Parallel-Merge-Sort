/*
 * File: mergesortparallel.c
 * Author: Katie Levy and Michael Poyatt
 * Date: 10/25/16
 * Description: Parallel mergesort algorithm
 */

#include <math.h>
#include "mergesortparallel.h"


/* Function Declarations */
void* threadFunc(void* rank);
int driverParallel(int start, int stop);
int mergeParallel(int start, int middle, int stop);
int binSearch(int arr[], int a, int b, int x);
void printParallel(int start, int stop);
int validateParallel();
int combine(long myRank);
void barrier();

/* Global variables */
extern int threadCount;
extern long n; //array size
extern int *vecSerial;
extern int *vecParallel;
extern int *temp;
pthread_t *threads;
int level;
extern int threads_ready;
extern pthread_cond_t ready_cv;
extern pthread_mutex_t lock;

// MergeSort a global array in parallel.
int mergeSortParallel() {
    // Allocate memory for threads
    threads = malloc(threadCount*sizeof(pthread_t));
    level = 1;
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
    long myFirsti, myFirstj, mySecondi, mySecondj;
    if (myRank < remainder) {
        myCount = quotient + 1;
        myFirsti = myRank * myCount;
    }
    else {
        myCount = quotient;
        myFirsti = myRank * myCount + remainder;
    }
    myFirstj = myFirsti + myCount;
    
    // Mergesort that section of the array
    driverParallel(myFirsti, myFirstj-1);

    int team_size, num_teams, chunk_size, block_size, team, offset, a, b;
    //double offset;
    barrier();

    while(pow(2, level) <= threadCount) {
        if(myRank == 0){
            printf("LEVEL %d\n", level);
        }
        barrier();

        // Each thread computes its 4 indices
        team_size = pow(2, level);
        num_teams = threadCount / team_size;
        chunk_size = n / (num_teams * 2);
        block_size = chunk_size / team_size;
        //printf("myRank=%ld, team_size=%d, divided=%ld\n", myRank, team_size, myRank/team_size);
        team = myRank / team_size;
        offset = team * chunk_size;
        //printf("thread %ld, team %d\n", myRank, team);
        //printf("thread %ld, offset %d\n", myRank, offset);
        myFirsti = offset + (myRank * block_size);
        myFirstj = myFirsti + block_size;
        printf("Thread %ld first: %ld --> %ld\n", myRank, myFirsti, myFirstj);
        // Odd levels copy from temp to arr, even arr to temp
        a = chunk_size * 2 * team;
        b = a + chunk_size;
        if((level % 2) == 0){
            mySecondi = binSearch(vecParallel, a, b, temp[myFirsti - 1]);
            mySecondj = binSearch(vecParallel, a, b, temp[myFirstj + 1]);
        }
        else{
            mySecondi = binSearch(temp, a, b, temp[myFirsti]);
            mySecondj = binSearch(temp, a, b, temp[myFirstj + 1]);
        }
        printf("Thread %ld second: %ld --> %ld\n", myRank, mySecondi, mySecondj);

        barrier();
        if(myRank == 0){
            level++;
        }
        barrier();
    }
    barrier();
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
    return b;
}

// Barrier to make threads wait for each other
void barrier(){
    pthread_mutex_lock(&lock);
    threads_ready++;
    if(threads_ready == threadCount){
        threads_ready = 0;
        pthread_cond_broadcast(&ready_cv);
    }
    else{
        while(pthread_cond_wait(&ready_cv, &lock) != 0);
    }
    pthread_mutex_unlock(&lock);
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
