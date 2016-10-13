
// Declare gloabal vars
extern int thread_count;
extern long size;

// MergeSort a global array in parallel.
int mergeSortParallel(void* rank) {
     // Get rank of this thread.
    long myRank = (long) rank;  /* Use long in case of 64-bit system */
    
    // Compute starting and ending indices for this thread to sum.
    long quotient = size / thread_count;
    long remainder = size % thread_count;
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
    
    // Serial mergesort portion and store in global partial sort array
    // TODO: change serial mergesort to noncontiguous subarrays 


    // Tree-based parallel reduction to merge sorted subarrays
    // TODO: write reduction

    return 0;
}