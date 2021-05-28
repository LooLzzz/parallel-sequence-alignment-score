#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "mpi.h"

int main(int argc, char *argv[])
{
    int rank; //process rank
    int x, n;
    MPI_Status status;

    srand(time(NULL)); // Initialization, should only be called once.
	MPI_Init(&argc, &argv); //start mpi
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (rank == 0)
    {
        //parent, sender
        printf("PARENT: my rank is %d\n", rank);

        x = random() % 100; //random number between [0,19]
        printf("PARENT: sending %d\n", x);
        MPI_Send(&x, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
        MPI_Recv(&x, 1, MPI_INT, 1, 0, MPI_COMM_WORLD, &status);
        printf("PARENT: received %d\n", x);
    }
    else
    {
        //child, reciever
        printf("CHILD: my rank is %d\n", rank);
        n = random() % 5; //random number between [0,19]

        MPI_Recv(&x, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
        printf("CHILD: received %d\n", x);
        printf("CHILD: sending %d*%d=%d\n", x, n, x*n);
        x *= n;
        MPI_Send(&x, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }
    
    MPI_Finalize(); //close mpi
    return 0;
}