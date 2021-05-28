#include "main.h"

#define LINE_MAX 255

/**
 * float A[N] = { ... }
 * float B[N] = { _empty }
 * 
 * for i in range(N):
 *     bi = [ sin(A[i] * cos(k)) for k in range(MAX_K) ]
 *     B[i] = max( b(i) )
 */

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
    // printFloatArr(arr, arrSize);




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

void printFloatArr(float *arr, int n)
{
    printf("arr[%d] = [ ", n);
    
    for (int i = 0; i < n-1; i++)
        printf("%.2f, ", arr[i]);
    
    printf("%.2f ]\n", arr[n-1]);
}
