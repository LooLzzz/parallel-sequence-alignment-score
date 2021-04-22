#include <stdio.h>
#include <string.h>
#include "mpi.h"

int main(int argc, char *argv[])
{
	MPI_Init(&argc, &argv); //start mpi
	
    int my_rank; //process rank
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    printf("my rank is %d\n", my_rank);
    
    MPI_Finalize(); //close mpi
    return 0;
}