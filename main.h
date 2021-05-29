#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <mpi.h>
#include <omp.h>

#include "utils.h"
#include "cudaFunctions.h"

#define LINE_MAX 255

int main(int argc, char *argv[]);

void readFloatArr(const char *filepath, float *arr[], int *arrSize);
void cpuCompute(float *A, int n, float *B, int K_MAX);
