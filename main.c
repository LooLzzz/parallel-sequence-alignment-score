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

    if (rank == 0)
    {
        // root
        
        // read values from 'input.dat'
        readFloatArr("input.dat", &arr, &arrSize);
        n = arrSize - (arrSize / 2);
        // printFloatArr(arr, arrSize);
        
        // send arrSize, let worker know allocate space for the arr
        MPI_Send(&arrSize, 1, MPI_INT, 1, MPI_ANY_TAG, MPI_COMM_WORLD);
        
        // send A[n:]
        MPI_Send(arr+(arrSize/2), n, MPI_INT, 1, MPI_ANY_TAG, MPI_COMM_WORLD);
    }
    else
    {
        //worker

        // recv arrSize from root
        MPI_Recv(&arrSize, 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        n = arrSize - (arrSize / 2);
        
        // realloc to fit the incoming A[n:]
        arr = (float *) realloc(arr, sizeof(float) * n);

        // recv A[n:]
        MPI_Recv(arr, arrSize-n, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
    }


    /**
     * n = N//2
     * `A[n:]` should be sent to another proccess using mpi
     * 
     * each half in each proccess should preform the following:
     * a[:n/2] -> compute on cpu
     * a[n/2:] -> compute on gpu
     */
    
    // send `A[:n]` using mpi

    // sendWithMpi(n) // the other proccess should know `n` as well
    // sendWithMpi(A[n:])

    // b1 = cpuCompute(A[:n/2]);
    // b2 = gpuCompute(A[n/2:n]);
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
