#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#define TRUE  1
#define FALSE 0

#define task_max(a,b) (a.score > b.score) ? a : b
#define task_min(a,b) (a.score < b.score) ? a : b
#define task_minmax(a,b,dir) dir ? task_max(a,b) : task_min(a,b)

void printFloatArr(float *arr, int n, const char *prefix, const char *arrName);
void freeAll(void *a, ...);
void freeMat(void **mat, int n);
