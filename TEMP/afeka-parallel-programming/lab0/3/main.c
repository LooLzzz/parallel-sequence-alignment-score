#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "mpi.h"

void print_arr(int *arr, int n)
{
    printf("{");
    for (int i = 0; i < n; i++)
        if (i != n-1)
            printf("%3d, ", arr[i]);
        else
            printf("%3d}\n", arr[i]); //last print
}

int main(int argc, char *argv[])
{
    int rank, i, val, N, pos_count;
    double start, end;
    int arr[1000];
    MPI_Status status;

    srand(time(NULL)); // Initialization, should only be called once.
	MPI_Init(&argc, &argv); //start mpi
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &N);
    N--;

    if (rank == 0)
    {
        //master
        start = MPI_Wtime();
        for (i = 0; i < N; i++)
        {
            arr[i] = random() % 100;
            arr[i] = (arr[i] > 50) ? -arr[i] : arr[i];
            MPI_Send(arr+i, 1, MPI_INT, i+1, 0, MPI_COMM_WORLD);
            // MPI_Recv(&val, 1, MPI_INT, i+1, 0, MPI_COMM_WORLD, &status);
            
            // if (val > 0)
            //     pos_count++;
        }
        for (i = 0; i < N; i++)
        {
            MPI_Recv(&val, 1, MPI_INT, i+1, 0, MPI_COMM_WORLD, &status);
            if (val > 0)
                pos_count++;
        }
        end = MPI_Wtime();

        printf("set:\n\t");
        print_arr(arr, N);

        printf("\npositive count: %d\n", pos_count);
        printf("negative count: %d\n", N-pos_count);
        
        printf("\nexec time: %.5f s", end-start);
    }
    else
    {
        //slaves
        MPI_Recv(&val, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
        val = (val > 0) ? 1 : -1;
        MPI_Send(&val, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }
    
    MPI_Finalize(); //close mpi
    return 0;
}