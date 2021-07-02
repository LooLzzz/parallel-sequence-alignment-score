#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <mpi.h>
#include <omp.h>

#include "utils.h"
#include "cudaFunctions.cuh"

#define OUTPUT_FILE "output.txt"

char AllLetters[] = "GPAVLIMCFYWHKRQNEDST";
char ConservativeGroups[CONSERVATIVE_GROUPS_COUNT][GROUP_MAX] = {
    "NDEQ", "NEQK", "STA",
    "MILV", "QHRK", "NHQK",
    "FYW", "HY", "MILF"
};
char SemiConservativeGroups[SEMI_CONSERVATIVE_GROUPS_COUNT][GROUP_MAX] = {
    "SAG", "ATV", "CSA",
    "SGND", "STPA", "STNK",
    "NEQHRK", "NDEQHK", "SNDEQK",
    "HFY", "FVLIM"
};


int main(int argc, char *argv[]);

void readInputsFromFile(const char *filepath, float weights[W_LEN], char seq1[SEQ1_MAXLEN], char seq2[SEQ2_MAXLEN], DIR *dir);
void generateMutantGroups(char MutantGroups[LETTER_COUNT][LETTER_COUNT+1]);
void generateAllMutants(char seq[], int seq_n, int *res_n, char ***res);
void generateTasks(char seq1[SEQ1_MAXLEN], char **seq2_mutants, int seq2_mutants_count, float weights[W_LEN], DIR dir, TASK *tasks, int tasks_count);

void printOutput(const char *outfile, TASK task);

// void computeTasks(TASK tasks[], int tasks_count, DIR dir);
// void computeSigns(TASK *task);
// void computeScore(TASK *task);
