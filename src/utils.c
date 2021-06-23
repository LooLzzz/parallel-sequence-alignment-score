#include "utils.h"

void printFloatArr(float *arr, int n, const char *prefix, const char *arrName)
{
    printf("%s%s[%d] = [ ", prefix, arrName, n);
    
    for (int i = 0; i < n-1; i++)
        printf("%.2f, ", arr[i]);
    
    printf("%.2f ]\n", arr[n-1]);
}

/**
 * Frees all passed variables
 * Last arg should be `NULL`
 */
void freeAll(void *a, ...)
{
    va_list args;
    va_start(args, a);

    do
    {
        free(a);
        a = va_arg(args, void*);
    }
    while (a);

    va_end(args);
}

void freeMat(void **mat, int n)
{
    while (n--)
        free(mat[n]);
    free(mat);
}
