#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <omp.h>

#include "utils.h"

#define LINE_MAX 255

int main(int argc, char *argv[]);

void readFloatArr(const char *filepath, float *arr[], int *arrSize);
