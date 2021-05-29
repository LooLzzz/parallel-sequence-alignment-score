#include "main.h"

/*
 * float A[N] = { ... }
 * float B[N] = { _empty }
 * 
 * for i in range(N):
 *     bi = [ sin(A[i] * cos(k)) for k in range(K_MAX) ]
 *     B[i] = max( b(i) )
 */

// global variables
int numOfProcesses;
int rank;

int main(int argc, char *argv[])
{
    // command line arguments validation (needed for K_MAX)
    if (argc < 2)
    {
        printf("expected 1 argument, instead got %d\n", argc);
        exit(EXIT_FAILURE);
    }

    // init values
    int n;
    int K_MAX = atoi(argv[1]);
    float *A = NULL;
    float *B = NULL;
    float *A_cpu, *A_gpu, *B_cpu, *B_gpu;
    char *workerPrefix;
    MPI_Status status;

    // init MPI
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numOfProcesses);
    if (numOfProcesses != 2)
    {
        printf("Run the example with two processes only\n");
        MPI_Abort(MPI_COMM_WORLD, __LINE__);
    }
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // init `A[]`
    if (rank == 0) // root
    {
        workerPrefix = "> root   > ";

        // read values from `input.dat`
        readFloatArr("input.dat", &A, &n);
        // printFloatArr(A, n, workerPrefix, "A_full");
        n /= 2;
        
        // send arrSize, let worker allocate space for `A[n:]`
        MPI_Send(&n, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
        
        // send `A[n:]`
        MPI_Send(A+n, n, MPI_FLOAT, 1, 0, MPI_COMM_WORLD);
    }
    else // worker
    {
        workerPrefix = "> worker > ";

        // recv arrSize from root
        MPI_Recv(&n, 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        
        // realloc to fit the incoming `A[n:]`
        A = (float*) realloc(A, n * sizeof(float));

        // receive `A[n:]` from root
        MPI_Recv(A, n, MPI_FLOAT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
    }

    // both proccesses have:
    // float A[n];
    // int n = len(A);
    // int arrSize = n*2;
    B = (float*) realloc(B, n * sizeof(float));

    A_cpu = A;
    A_gpu = A + n/2;
    B_cpu = B;
    B_gpu = B + n/2;

    // printFloatArr(A, n, workerPrefix, "A");
    // printFloatArr(A_cpu, n/2, workerPrefix, "A_cpu");
    // printFloatArr(A_gpu, n/2, workerPrefix, "A_gpu");

    #pragma omp parallel sections
    {
        #pragma omp section
            cpuCompute(A_cpu, n/2, B_cpu, K_MAX);
        
        #pragma omp section
            gpuCompute(A_gpu, n/2, B_gpu, K_MAX);
    }

    // printFloatArr(B, n, workerPrefix, "B");
    MPI_Barrier(MPI_COMM_WORLD);

    if (rank == 0)
    {
        // root receiving from worker
        B = (float*) realloc(B, n * 2 * sizeof(float));

        // get `B_full[n:]` from worker
        MPI_Recv(B+n, n, MPI_FLOAT, 1, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
    
        printFloatArr(B, n*2, workerPrefix, "B_final");
    }
    else
    {
        // worker sending to root
        MPI_Send(B, n, MPI_FLOAT, 0, 0, MPI_COMM_WORLD);
    }

    MPI_Finalize();
    freeAll(A, B, NULL);
    return EXIT_SUCCESS;
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
        // sscanf(line, "%f", &A[arrSize-1]);
    }

    fclose(fp);
}


/**
 * use OpenMP to compute array B
 */
void cpuCompute(float *A, int n, float *B, int K_MAX)
{
    int i, k;
    float val;

    // printFloatArr(A, n, " > cpuCompute() > ", "A");
    // printf("K_MAX = %d\n", K_MAX);

    #pragma omp parallel for private(i, k, val) num_threads(n)
        for (i = 0; i < n; i++)
        {
            B[i] = __FLT_MIN__; // smallest float value
            for (k = 0; k < K_MAX; k++)
            {
                val = sin( A[i] * cos(k) );
                B[i] = max(val, B[i]);
            }
        }
    
    // printFloatArr(B, n, " > cpuCompute() > ", "B");
}
