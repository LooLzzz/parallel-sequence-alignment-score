#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <mpi.h>
#include <omp.h>

#include "utils.h"
// #include "cudaFunctions.h"

#define LINE_MAX 255
#define SEQ1_MAXLEN 10000
#define SEQ2_MAXLEN 5000
#define W_LEN 4

#define max(a,b) (a > b) ? a : b
#define min(a,b) (a < b) ? a : b

typedef enum _DIR
{
    MIN = 0,
    MAX
} DIR;

int main(int argc, char *argv[]);

void readInputsFromFile(const char *filepath, float (*W)[W_LEN], char (*seq1)[SEQ1_MAXLEN], char (*seq2)[SEQ2_MAXLEN], DIR *dir);
char** generateAllMutants(char *seq, int *n);

void cpuCompute();
