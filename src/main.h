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

#define LETTER_COUNT 20
#define CONSERVATIVE_GROUPS_COUNT 9
#define SEMI_CONSERVATIVE_GROUPS_COUNT 11

typedef enum _DIR
{
    MIN = 0,
    MAX
} DIR;

typedef enum _LETTERS
{
    G=0, P, A, V, L,
    I, M, C, F, Y,
    W, H, K, R, Q,
    N, E, D, S, T
} LETTERS;

char AllLetters[] = "GPAVLIMCFYWHKRQNEDST";

char ConservativeGroups[CONSERVATIVE_GROUPS_COUNT][5] = {
    "NDEQ", "NEQK", "STA",
    "MILV", "QHRK", "NHQK",
    "FYW", "HY", "MILF"
};

char SemiConservativeGroups[SEMI_CONSERVATIVE_GROUPS_COUNT][7] = {
    "SAG", "ATV", "CSA",
    "SGND", "STPA", "STNK",
    "NEQHRK", "NDEQHK", "SNDEQK",
    "HFY", "FVLIM"
};


int main(int argc, char *argv[]);

void readInputsFromFile(const char *filepath, float (*W)[W_LEN], char (*seq1)[SEQ1_MAXLEN], char (*seq2)[SEQ2_MAXLEN], DIR *dir);
void generateMutantGroups(char MutantGroups[LETTER_COUNT][LETTER_COUNT+1]);
void generateAllMutants(char seq[], int seq_n, int *res_n, char **res);

void cpuCompute();
