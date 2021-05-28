#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <omp.h>

#define LINE_SIZE 255

int main(int argc, char *argv[]);

void readFloatArr(const char *filepath, float *arr[], int *arrSize);
void printFloatArr(float *arr, int n);
