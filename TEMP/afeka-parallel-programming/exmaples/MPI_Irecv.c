#include "mpi.h"
#include <stdio.h>
#define FOREVER 1

void task(double sec)
{
    double t = MPI_Wtime();
    // Loop till sec seconds will pass
    while (MPI_Wtime() - t < sec) {}
}

int main(int argc, char *argv[])
{
    int myid, numprocs, flag, tasksPerformed;
    int data;
    MPI_Request request;
    MPI_Status status;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &myid);

    if (myid == 0)
    {
        task(10);
        data = 123;
        MPI_Send(&data, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
    }
    else
    {
        MPI_Irecv(&data, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &request);
        MPI_Test(&request, &flag, &status);
        tasksPerformed = 0;
        while ((flag == 0) && (tasksPerformed < 7))
        {
            task(1);
            tasksPerformed++;
            MPI_Test(&request, &flag, &status);
        }
        MPI_Wait(&request, &status);
        printf("Received %d, performed %d additional tasks\n", data, tasksPerformed);
    }

    MPI_Finalize();
    return 0;
}
