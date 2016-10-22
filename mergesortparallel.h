#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include <sys/time.h>

/* Shared Function Declaraions */
int mergeSortParallel();

/* Global variables */
extern int threadCount;
extern long n; //array size
extern int *vecSerial;
extern int *vecParallel;
extern int *temp;
