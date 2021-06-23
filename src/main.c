#include "main.h"

// global variables
int numOfProcesses;
int rank;

#define INPUT_FILE "input.txt"

int main(int argc, char *argv[])
{
    // init values
    MPI_Status status;
    char workerPrefix[20];

    float W[W_LEN];
    char seq1[SEQ1_MAXLEN];
    char seq2[SEQ2_MAXLEN];
    DIR dir;

    int num_of_mutants = 0;
    char **seq2_mutants;

    // init MPI
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numOfProcesses);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    // if (numOfProcesses != 2)
    // {
    //     printf("Run the example with two processes only\n");
    //     MPI_Abort(MPI_COMM_WORLD, __LINE__);
    // }


    if (rank == 0) // root
    {
        sprintf(workerPrefix, ">  root   > ");
        readInputsFromFile(INPUT_FILE, &W, &seq1, &seq2, &dir);

        MPI_Send(W, W_LEN, MPI_FLOAT, 1, 0, MPI_COMM_WORLD); // W
        MPI_Send(seq1, SEQ1_MAXLEN, MPI_CHAR, 1, 0, MPI_COMM_WORLD); // seq1
        MPI_Send(&dir, 1, MPI_INT, 1, 0, MPI_COMM_WORLD); // dir

        seq2_mutants = generateAllMutants(seq2, &num_of_mutants);
    }
    else // worker
    {
        sprintf(workerPrefix, "> worker%d > ", rank);

        MPI_Recv(W, W_LEN, MPI_FLOAT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status); // W
        MPI_Recv(seq1, SEQ1_MAXLEN, MPI_CHAR, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status); // seq1
        MPI_Recv(&dir, 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status); // dir
    }

    MPI_Finalize();
    // free(seq2_mutants);
    freeMat((void**)seq2_mutants, num_of_mutants);
    return EXIT_SUCCESS;
}

void readInputsFromFile(const char *filepath, float (*W)[W_LEN], char (*seq1)[SEQ1_MAXLEN], char (*seq2)[SEQ2_MAXLEN], DIR *dir)
{
    char line[LINE_MAX];
    FILE *fd = fopen(filepath, "r");

    fscanf(fd, "%f %f %f %f", *W, (*W)+1, (*W)+2, (*W)+3);
    fscanf(fd, "%s", *seq1);
    fscanf(fd, "%s", *seq2);
    fscanf(fd, "%s", line); //dir

    if (strcasecmp(line, "maximum"))
        *dir = MAX;
    else
        *dir = MIN;

    fclose(fd);

    // // DEBUG
    // printFloatArr(*W, 4, "", "W");
    // printf("seq1 = %s\n", *seq1);
    // printf("seq2 = %s\n", *seq2);
    // printf("dir = %s\n", *dir==0 ? "MIN" : "MAX");
    // // DEBUG
}

char** generateAllMutants(char *seq, int *n)
{
    int count = 0;
    char **all_mutants = NULL;
    char new_seq[SEQ2_MAXLEN];

    //TODO

    *n = count;
}

void cpuCompute()
{
    
}
