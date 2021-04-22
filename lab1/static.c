#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>

#include "heavy.c"

void master(int rank, int num_of_workers, int num_of_pairs)
{
    int i, j, count, *arr, *buff_send, offset=0;
    double heavy_result, answer, start, end;
    MPI_Status status;

    start = MPI_Wtime();

    count = 0;
    arr = malloc(sizeof(int) * SIZE*SIZE*2);
    for (i=0; i < SIZE; i++)
        for (j=0; j < SIZE; j++)
        {
            arr[count] = i;
            arr[count+1] = j;
            count += 2;
        }
    
    // printf("> master: mod is %d\n", mod);
    // printf("> master: sending %d pairs -> %d items\n", num_of_pairs, num_of_pairs*2);
    // printf("> master: first pair is (%d,%d)\n", send_arr[0], send_arr[1]);
    
    i = 1;
    if (num_of_pairs % num_of_workers != 0)
    {
        // the first worker will get extra pairs
        offset = (num_of_pairs + (num_of_pairs%num_of_workers))*2;
        MPI_Send(arr, offset, MPI_INT, i, 0, MPI_COMM_WORLD);
        
        i++;
    }

    // sending
    for (; i <= num_of_workers; i++)
    {
        buff_send = arr + offset + ((i-1) * num_of_pairs*2);
        MPI_Send(buff_send, num_of_pairs*2, MPI_INT, i, 0, MPI_COMM_WORLD);
    }
    
    // recieving
    for (i=0; i < num_of_workers; i++)
    {
        MPI_Recv(&heavy_result, 1, MPI_DOUBLE, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
        answer += heavy_result;
        
        printf("> master: got result from %d -> %.2f\n", status.MPI_SOURCE, heavy_result);
    }
    end = MPI_Wtime();

    printf("> master: final answer = %.5f\n", answer);
    printf("> master: exec time = %.2f seconds\n", end-start);

    free(arr);
}

void worker(int rank)
{
    int x, y, i, count, *arr;
    double heavy_result;
    MPI_Status status;

    MPI_Probe(0, 0, MPI_COMM_WORLD, &status);
    MPI_Get_count(&status, MPI_INT, &count);

    arr = malloc(sizeof(int) * count);
    MPI_Recv(arr, count, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
    printf("> worker %d: got %d items from master\n", rank, count);
    
    for (i=0; i < count; i+=2)
    {
        // printf("> worker %d: got arr[%d:%d] (%d,%d)\n", rank, i, i+1, arr[i], arr[i+1]);
        x = arr[i];
        y = arr[i+1];
        
        heavy_result += heavy(x, y);
    }
    
    printf("> worker %d: sending result %.2f\n", rank, heavy_result);
    MPI_Send(&heavy_result, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);

    free(arr);
}

int main(int argc, char *argv[])
{
	int rank, num_of_processes, num_of_workers, num_of_pairs;

	MPI_Init(&argc, &argv); //start mpi
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &num_of_processes);

    num_of_pairs = (SIZE*SIZE) / (num_of_processes-1);
    num_of_workers = num_of_processes-1;

    if (rank == 0)
        master(rank, num_of_workers, num_of_pairs);
    else
        worker(rank);
    
    MPI_Finalize(); //close mpi
    return 0;
}