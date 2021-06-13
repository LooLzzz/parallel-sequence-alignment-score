#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#define LINE_MAX 255

#define max(a,b) (a > b) ? a : b
#define min(a,b) (a < b) ? a : b

void printFloatArr(float *arr, int n, const char *prefix, const char *arrName);
void freeAll(void *a, ...);
