#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <mpi.h>

#include "heavy.c"

typedef struct _task_s
{
    int x;
    int y;
} task_s;

MPI_Datatype COMMIT_MPI_TASK_TYPE()
{
    int nitems = 2;
    int blocklengths[2] = {1, 1};
    MPI_Datatype types[2] = {MPI_INT, MPI_INT};
    MPI_Datatype MPI_TASK_TYPE;
    MPI_Aint offsets[2];

    offsets[0] = offsetof(task_s, x);
    offsets[1] = offsetof(task_s, y);

    MPI_Type_create_struct(nitems, blocklengths, offsets, types, &MPI_TASK_TYPE);
    MPI_Type_commit(&MPI_TASK_TYPE);

    return MPI_TASK_TYPE;
}

void master(int rank, int num_of_workers)
{
    int i, j, count;
    double result, answer, start, end;
    task_s *tasks, task;
    MPI_Status status;
    MPI_Datatype MPI_TASK_TYPE = COMMIT_MPI_TASK_TYPE();
    
    count = 0;
    start = MPI_Wtime();
    tasks = malloc(sizeof(task_s) * SIZE*SIZE);

    for (i=0; i < SIZE; i++)
        for (j=0; j < SIZE; j++)
        {
            tasks[count].x = i;
            tasks[count].y = j;
            count++;
        }
    
    // printf("> master: first pair is (%d,%d)\n", tasks[0].x, tasks[0].y);

    // launching some tasks first, so the workers will start working on them
    for (i=0; i < num_of_workers; i++)
    {
        printf("> master: sending job {%d,%d}\n", tasks[i].x, tasks[i].y);
        MPI_Send(&tasks[i], 1, MPI_TASK_TYPE,
            i+1, 0, MPI_COMM_WORLD
        );
    }

    count = 0;
    while (count < SIZE*SIZE)
    {
        // waiting for the results
        MPI_Recv(&result, 1, MPI_DOUBLE, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        
        printf("> master: got result from %d -> %.2f\n" \
               "> master: res count %d -> %d\n",
               status.MPI_SOURCE, result, count, count+1
        );

        if (count > (SIZE*SIZE)-num_of_workers-1)
        {
            // no tasks left, stop workers
            printf("> master: stopping worker %d\n", status.MPI_SOURCE);
            MPI_Send(NULL, 0, MPI_TASK_TYPE,
                status.MPI_SOURCE, MPI_ERR_OTHER, MPI_COMM_WORLD
            );
        }
        else
        {
            // keep sending tasks until no tasks are left
            printf("> master: sending job {%d,%d}\n", tasks[count+num_of_workers].x, tasks[count+num_of_workers].y);
            MPI_Send(&tasks[count+num_of_workers], 1, MPI_TASK_TYPE,
                status.MPI_SOURCE, 0, MPI_COMM_WORLD
            );
        }
        
        count++;
        answer += result;
    }

    end = MPI_Wtime();
    printf("> master: final answer = %.5f\n", answer);
    printf("> master: exec time = %.2f seconds\n", end-start);

    free(tasks);
}

void worker(int rank)
{
    double result;
    task_s task;
    MPI_Datatype MPI_TASK_TYPE = COMMIT_MPI_TASK_TYPE();
    MPI_Status status;

    while (1)
    {
        printf("> worker %d: waiting for task..\n", rank);
        MPI_Recv(&task, 1, MPI_TASK_TYPE, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        
        if (status.MPI_TAG == MPI_ERR_OTHER)
        {
            printf("> worker %d: got a stop tag, exiting..\n", rank);
            break;
        }

        printf("> worker %d: got job {%d,%d}\n", rank, task.x, task.y);
        result = heavy(task.x, task.y);
        
        printf("> worker %d: sending result %.2f\n", rank, result);
        MPI_Send(&result, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
    }
}

int main(int argc, char *argv[])
{
	int rank, num_of_processes, num_of_workers;

	MPI_Init(&argc, &argv); //start mpi
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &num_of_processes);

    num_of_workers = num_of_processes-1;

    if (rank == 0)
        master(rank, num_of_workers);
    else
        worker(rank);
    
    MPI_Finalize(); //close mpi
    return 0;
}