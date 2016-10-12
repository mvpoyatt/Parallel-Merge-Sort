#include <stdio.h>
#include <stdlib.h>

int *arr;

int main(){
    arr = (int*)malloc(sizeof(int)*100);
    int i;
    for(i = 0; i < 100; i++){
        arr[i] = rand();
        printf("%d", arr[i]);
    }
    int result = validate(arr, 100);
    printf("\n%d\n", result);
    exit(0);
}


int validate(int *array, int size){
    int i;
    for(i = 0; i < size-1; i++){
        if(array[i] > array[i+1]){
            return 1;
        }
    }
    return 0;
}
# Parallel-Merge-Sort
