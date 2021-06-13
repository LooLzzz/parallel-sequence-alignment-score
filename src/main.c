#include "main.h"

// global variables
int numOfProcesses;
int rank;

int main(int argc, char *argv[])
{
    // init values
    MPI_Status status;
    char workerPrefix[20];
    int n, i;

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
        strcpy(workerPrefix, ">  root   > ");
        
        // readInputsFromFile();
    }
    else // worker
    {
        sprintf(workerPrefix, "> worker%d > ", rank);
    }


    MPI_Finalize();
    // freeAll(A, B, NULL);
    return EXIT_SUCCESS;
}

void readInputsFromFile(const char *filepath, float *W[], char *seq1[], char *seq2[], DIR *dir)
{
    // char line[LINE_MAX];
    // FILE *fd = fopen(filepath, "r");
}

void cpuCompute()
{
    
}
