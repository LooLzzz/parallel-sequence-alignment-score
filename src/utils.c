#include "utils.h"

void printFloatArr(float *arr, int n, const char *prefix, const char *arrName)
{
    printf("%s%s[%d] = [ ", prefix, arrName, n);
    
    for (int i = 0; i < n-1; i++)
        printf("%.2f, ", arr[i]);
    
    printf("%.2f ]\n", arr[n-1]);
}

void printTask(TASK task, const char *taskName)
{
    char prefix_spaces[256] = {0};
    memset(prefix_spaces, ' ', task.offset);

    printf("%s = {\n", taskName);
    printf("  seq1       = %s\n", task.seq1);
    printf("  seq2       = %s%s\n", prefix_spaces, task.seq2);
    printf("  signs      = %s%s\n", prefix_spaces, task.signs);
    printf("  offset     = %d\n", task.offset);
    printFloatArr(task.weights, 4, "  ", "weights");
    printf("  score      = %.2f\n", task.score);
    printf("  dir        = %s\n", task.dir ? "MAX" : "MIN");
    printf("}\n");
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
