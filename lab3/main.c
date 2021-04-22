#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

typedef enum DIR {
    DEC,
    ASC
} DIR;

#define RAND_ARR_MAX 100
#define max(a,b) (a > b ? a : b)
#define min(a,b) (a < b ? a : b)
#define minmax(a,b,dir) (dir ? max(a,b) : min(a,b))

int num_of_processes, rank;

int oddEvenSort(int *val, int pivot, int next, int prev, DIR dir)
{
    int last, i, a, b;
    MPI_Status status;

    last = num_of_processes-1;
    a = *val;

    for (i = 0; i < num_of_processes; i++)
    {
        if (pivot % 2 == 0)
        {
            if (i % 2 == 0)
            {
                MPI_Send(&a, 1, MPI_INT, next, 0, MPI_COMM_WORLD);
                MPI_Recv(&b, 1, MPI_INT, next, 0, MPI_COMM_WORLD, &status);
                a = minmax(a, b, dir);
            }
            else if (pivot != 0 && pivot != last)
            {
                MPI_Send(&a, 1, MPI_INT, prev, 0, MPI_COMM_WORLD);
                MPI_Recv(&b, 1, MPI_INT, prev, 0, MPI_COMM_WORLD, &status);
                a = minmax(a, b, !dir);
            }
         
        }
        else
        {
            if (i % 2 == 0)
            {
                MPI_Send(&a, 1, MPI_INT, prev, 0, MPI_COMM_WORLD);
                MPI_Recv(&b, 1, MPI_INT, prev, 0, MPI_COMM_WORLD, &status);
                a = minmax(a, b, !dir);
            }
            else if (pivot != 0 && pivot != last)
            {
                MPI_Send(&a, 1, MPI_INT, next, 0, MPI_COMM_WORLD);
                MPI_Recv(&b, 1, MPI_INT, next, 0, MPI_COMM_WORLD, &status);
                a = minmax(a, b, dir);
            }
        }
    }

    *val = a;
    return a;
}

void print_arr(int *arr, int len)
{
    for (int i = 0; i < len; i++)
        printf("%-4d", arr[i]);
    
    printf("\n");
}

int main(int argc, char *argv[])
{
    int val, i;
    int *arr = NULL;
    int *sorted = NULL;

	MPI_Init(&argc, &argv); //start mpi
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &num_of_processes);

    if (rank == 0)
    {
        // root sending `scatter()` to workers
        arr = malloc(sizeof(int)*num_of_processes);
        sorted = malloc(sizeof(int)*num_of_processes);

        srand(time(NULL));
        for (i = 0; i < num_of_processes; i++)
            arr[i] = rand() % RAND_ARR_MAX; //random num between [0, max_val]

        MPI_Scatter(arr, 1, MPI_INT, &val, 1, MPI_INT, 0, MPI_COMM_WORLD);
    }
    else
        // workers receiving `scatter()` from root
        MPI_Scatter(NULL, 0, MPI_INT, &val, 1, MPI_INT, 0, MPI_COMM_WORLD);
    
    // sort this shiz
    // oddEvenSort(&val, rank, rank+1, rank-1, DEC);
    oddEvenSort(&val, rank, rank+1, rank-1, ASC);
    
    if (rank == 0)
        // root receiving `gather()` from workers
        MPI_Gather(&val, 1, MPI_INT, sorted, 1, MPI_INT, 0, MPI_COMM_WORLD);
    else
        // workers sending `gather()` to root
        MPI_Gather(&val, 1, MPI_INT, NULL, 0, MPI_INT, 0, MPI_COMM_WORLD);

    if (rank == 0)
    {
        printf("\noriginal:\n");
        print_arr(arr, num_of_processes);

        printf("\nsorted:\n");
        print_arr(sorted, num_of_processes);
    }
    
    // printf("\n{%d} -> val=%d\n", rank, val);

    free(arr);
    free(sorted);
    MPI_Finalize(); //close mpi
    return 0;
}








// #include <stdio.h>
// #include <stdlib.h>
// #include <time.h>
// #include <mpi.h>

// typedef enum DIR {
//     RTL,
//     LTR
// } DIR;

// int num_of_processes;

// int max(int a, int b) { return a > b ? a : b; }
// int min(int a, int b) { return a < b ? a : b; }

// void oddEvenSort(int *val, int location, int hi, int lo, DIR dir)
// {
//     int i, val_result;
//     int (*f1)(int,int);
//     int (*f2)(int,int);
//     MPI_Status status;

//     int last = num_of_processes-1;

//     if (dir == RTL)
//     {
//         f1 = &min;
//         f2 = &max;
//     }
//     else
//     {
//         f1 = &max;
//         f2 = &min;
//     }
    
//     for (i = 0; i < num_of_processes; i++)
//     {
//         if (location % 2 == 0)
//         {
//             //P2
//             if (i % 2 == 0)
//             {
//                 MPI_Send(val, 1, MPI_INT, hi, 0, MPI_COMM_WORLD);
//                 MPI_Recv(&val_result, 1, MPI_INT, hi, 0, MPI_COMM_WORLD, &status);
//                 *val = (*f1)(*val, val_result);
//             }
//             else if (location != 0 && location != last)
//             {
//                 MPI_Send(val, 1, MPI_INT, lo, 0, MPI_COMM_WORLD);
//                 MPI_Recv(&val_result, 1, MPI_INT, lo, 0, MPI_COMM_WORLD, &status);                
//                 *val = (*f2)(*val, val_result);
//             }
//         }
//         else
//         {
//             //P3
//             if (i % 2 == 0)
//             {
//                 MPI_Send(val, 1, MPI_INT, lo, 0, MPI_COMM_WORLD);
//                 MPI_Recv(&val_result, 1, MPI_INT, lo, 0, MPI_COMM_WORLD, &status);
//                 *val = (*f2)(*val, val_result);
//             }
//             else if (location != 0 && location != last)
//             {
//                 MPI_Send(val, 1, MPI_INT, hi, 0, MPI_COMM_WORLD);
//                 MPI_Recv(&val_result, 1, MPI_INT, hi, 0, MPI_COMM_WORLD, &status);
//                 *val = (*f1)(*val, val_result);
//             }
//         }
//     }
// }

// void print_arr(int *arr, int len)
// {
//     for (int i = 0; i < len; i++)
//         printf("%-4d", arr[i]);
    
//     printf("\n");
// }

// int main(int argc, char *argv[])
// {
//     int rank, num_of_processes, val, i;
//     int *arr = NULL;
//     int *sorted = NULL;

// 	MPI_Init(&argc, &argv); //start mpi
// 	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
//     MPI_Comm_size(MPI_COMM_WORLD, &num_of_processes);

//     if (rank == 0)
//     {
//         // root sending `scatter()` to workers
//         arr = malloc(sizeof(int)*num_of_processes);
//         sorted = malloc(sizeof(int)*num_of_processes);

//         srand(time(NULL));
//         for (i = 0; i < num_of_processes; i++)
//             arr[i] = rand() % 50; //random num between [0, 100]

//         MPI_Scatter(arr, 1, MPI_INT, &val, 1, MPI_INT, 0, MPI_COMM_WORLD);
//     }
//     else
//         // workers receiving `scatter()` from root
//         MPI_Scatter(NULL, 0, MPI_INT, &val, 1, MPI_INT, 0, MPI_COMM_WORLD);
    
//     // sort this shiz
//     oddEvenSort(&val, rank, rank+1, rank-1, RTL);
    
//     if (rank == 0)
//         // root receiving `gather()` from workers
//         MPI_Gather(&val, 1, MPI_INT, sorted, 1, MPI_INT, 0, MPI_COMM_WORLD);
//     else
//         // workers sending `gather()` to root
//         MPI_Gather(&val, 1, MPI_INT, NULL, 0, MPI_INT, 0, MPI_COMM_WORLD);

//     if (rank == 0)
//     {
//         printf("original:\n");
//         print_arr(arr, num_of_processes);

//         printf("\nsorted:\n");
//         print_arr(sorted, num_of_processes);
//     }

//     free(arr);
//     free(sorted);
//     MPI_Finalize(); //close mpi
//     return 0;
// }