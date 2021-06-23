#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

void printFloatArr(float *arr, int n, const char *prefix, const char *arrName);
void freeAll(void *a, ...);
void freeMat(void **mat, int n);
