#include "main.h"

/**
 * float A[N] = { ... }
 * float B[N] = { _empty }
 * 
 * for i in range(N):
 *     bi = [ sin(A[i] * cos(k)) for k in range(MAX_K) ]
 *     B[i] = max( b(i) )
 */

// global variables
int numOfProcesses;
int rank;

int main(int argc, char *argv[])
{
    // command line arguments validation (needed for MAX_K)
    if (argc < 2)
    {
        printf("expected 1 argument, instead got %d\n", argc);
        exit(EXIT_FAILURE);
    }

    // init values
    int n;
    int MAX_K = atoi(argv[1]);
    int arrSize = 0;
    float *arr = NULL;
    MPI_Status status;

    // init MPI
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numOfProcesses);
    // if (numOfProcesses != 2)
    // {
    //     printf("Run the example with two processes only\n");
    //     MPI_Abort(MPI_COMM_WORLD, __LINE__);
    // }
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // read array values from 'input.dat'
    readFloatArr("input.dat", &arr, &arrSize);
    n = arrSize / 2;
    // printFloatArr(arr, arrSize);


    /**
     * n = N//2
     * `A[n:]` should be sent to another proccess using mpi
     * 
     * each half in each proccess should preform the following:
     * a[:n/2] -> compute on cpu
     * a[n/2:] -> compute on gpu
     */
    
    // send `A[:n]` using mpi

    // b1 = cpuCompute(A[:n/2]);
    // b2 = gpuCompute(A[n/2:]);
    // b3 = from mpi
    // b4 = from mpi
    
    // B = concat [b1, b2, b3, b4]


    MPI_Finalize();
    free(arr);
    exit(EXIT_SUCCESS);
}

void readFloatArr(const char *filepath, float *arr[], int *arrSize)
{
    char line[LINE_MAX];
    FILE *fp = fopen(filepath, "r");
    *arrSize = 0;
    
    while (fgets(line, LINE_MAX, fp))
    {
        *arr = (float*) realloc(*arr, ++(*arrSize) * sizeof(float));
        (*arr)[*(arrSize)-1] = atof(line);
        // sscanf(line, "%f", &arr[arrSize-1]);
    }

    fclose(fp);
}
