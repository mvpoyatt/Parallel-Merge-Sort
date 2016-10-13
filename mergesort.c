/*
 * File: mergesort.c
 * Author: Katie Levy and Michael Poyatt
 * Date: 10/10/16
 * Description: Mergesort algorithm timing both serial and 
 * parallel execution
 */


#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include <sys/time.h>
#include"mergesortparallel.h"


/* Function declarations */
int serialsort(int size);
int mergeSort(int start, int stop);
int merge(int start, int middle, int stop);
int validateSerialSort();
void printArray(int arr[], int size);

/* Global variables */
int *arr1;
int *arr2;
int *temp;

/*--------------------------------------------------------------------*/
int main(int argc, char* argv[]){
   // Check for 2 command line args
   if(argc != 3){
       printf("You need to enter 2 command line arguments to run this program");
       exit(0);
   }
   
   // Parse command line args
    size = strtol(argv[1], NULL, 10);
    thread_count = (int) strtol(argv[2], NULL, 10);
    printf("size %ld threads %d\n", size, thread_count);

    // For timing
    struct timeval  tv1, tv2;

    // Allocate memory for global arrays
    arr1 = (int *) malloc(sizeof(int) * size);
    arr2 = (int *) malloc(sizeof(int) * size);   
    temp = (int *) malloc(sizeof(int) * size);   
    int i; 

    // Fill the arrays with the same random numbers
    srand(time(NULL));
    for(i = 0; i < size; i++){
        int random = rand() % 100;
        arr1[i] = random;
    }

    // Copy first array to second array
    memcpy(arr2, arr1, sizeof(int)*size);
    memcpy(temp, arr1, sizeof(int)*size);

    //Perform the mergesort
    printf("Original Array: \n");
    printArray(arr1, size);
    printf("Serial Sorted Array\n");    
    gettimeofday(&tv1, NULL); // start timing
    serialsort(size);
    gettimeofday(&tv2, NULL); // stop timing
    double serialTime = (double) (tv2.tv_usec - tv1.tv_usec) / 1000000 +
        (double) (tv2.tv_sec - tv1.tv_sec);
    
     // Print results.
    printf("Serial time = %e\n", serialTime);
    free(arr1);
    free(arr2);
    free(temp);
    return 0;
}

// Returns 0 on success and 1 on failure
int serialsort(int size){
    if(!(mergeSort(0, size -1))){
        printArray(temp, size);
    }else{
        return 1;
    }
    return 0;
}

int mergeSort(int start, int stop){
//    printf("MergeSort start %d stop %d \n", start, stop);
    if(start >= stop){
        return 0;
    }
    int middle = ((stop + start) / 2);
    mergeSort(start, middle);
    mergeSort(middle+1, stop);
    merge(start, middle, stop);
    return 0;
}

int merge(int start, int middle, int stop){
//    printf("merge start %d middle %d stop %d \n", start, middle, stop);
    int first = start;
    int second = middle+1;
    int tempIndex = start;
    while(first <= middle && second <= stop){
//        printf("comparing %d and %d\n", arr1[first], arr1[second]);
        if(arr1[first] < arr1[second]){
            temp[tempIndex] = arr1[first];
            first++;
            tempIndex++;
        } else {
            temp[tempIndex] = arr1[second];
            second++;
            tempIndex++;
        }
//        printf("%d\n", temp[tempIndex-1]);
    }
    while(first <= middle){
        temp[tempIndex] = arr1[first];
            first++;
            tempIndex++;
    }
    while(second <= stop){
        temp[tempIndex] = arr1[second];
            second++;
            tempIndex++;
    }
    for(int i = start; i <= stop; i++){
        arr1[i] = temp[i];
    }
    return 0;
}


int validateSerialSort(){
    return 0;
}

void printArray(int arr[], int size){  
    int i;
    for(i = 0; i < size; i++){
        printf("%d\t", arr[i]);
    }
    printf("\n");
    return;
}