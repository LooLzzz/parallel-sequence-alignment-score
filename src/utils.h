#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

typedef struct _TASK
{
    char seq1[];
    char seq2[];
    int offset;
    float weights[];
    char signs[];
    float score;
} TASK;

#define task_max(a,b) (a.score > b.score) ? a : b
#define task_min(a,b) (a.score < b.score) ? a : b
#define task_minmax(a,b,dir) dir ? task_max(a,b) : task_min(a,b)

void printFloatArr(float *arr, int n, const char *prefix, const char *arrName);
void printTask(TASK task, const char *taskName);
void freeAll(void *a, ...);
void freeMat(void **mat, int n);
