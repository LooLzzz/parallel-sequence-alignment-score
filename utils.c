#include "utils.h"

void printFloatArr(float *arr, int n)
{
    printf("arr[%d] = [ ", n);
    
    for (int i = 0; i < n-1; i++)
        printf("%.2f, ", arr[i]);
    
    printf("%.2f ]\n", arr[n-1]);
}
