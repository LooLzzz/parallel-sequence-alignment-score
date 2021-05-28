#include "main.h"

int main(int argc, char *argv[])
{
    // char file_path[MAXCHAR] = argv[1];
    int num_of_processes, rank, num_of_cuboids, i, max_phases, next, prev, row_wise;
    int cart_rank, reorder, row, column, prev_row, next_row, prev_col, next_col;
    int dim[2], period[2], coords[2];
    DIR sort_dir, dir;
    MPI_Comm comm;
    MPI_Datatype MPI_CUBOID_TYPE;
    CUBOID val, *cuboids = NULL;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &num_of_processes);
    MPI_CUBOID_TYPE = COMMIT_MPI_CUBOID_TYPE();
    
    period[0] = 0;
    period[1] = 0;
    reorder = 1;

    if (rank == 0)
    {
        get_cubes_from_file(argv[1], num_of_processes, &num_of_cuboids, &sort_dir, &cuboids);
        
        dim[0] = dim[1] = (int) sqrt(num_of_cuboids);

        printf("before:\n");
        print_arr(cuboids, num_of_cuboids, dim[0]);
        printf("\n");
    }
    
    // pass {dim[], sort_dir, num_of_cuboids} to all processes
    MPI_Bcast(dim, 2, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&num_of_cuboids, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&sort_dir, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // create cartesian coords system
    MPI_Cart_create(MPI_COMM_WORLD, 2, dim, period, reorder, &comm);
    MPI_Cart_coords(comm, rank, 2, coords);
    MPI_Cart_rank(comm, coords, &cart_rank);
    MPI_Cart_shift(comm, 0, 1, &prev_col, &next_col); //shift cols
    MPI_Cart_shift(comm, 1, 1, &prev_row, &next_row); //shift rows
    
    MPI_Scatter(cuboids, 1, MPI_CUBOID_TYPE, &val, 1, MPI_CUBOID_TYPE, 0, comm);

    // (2log(n)+1) row/column phases are need to sort n^2 cuboids
    max_phases = (int) (2*log2(dim[0])) + 1;
    for (i = 0; i < max_phases; i++)
    {
        if (i % 2 == 0) // row-wise, snake-like directions
        {
            row_wise = 1;
            next = next_row;
            prev = prev_row;
            if (coords[0] % 2 == 0)
                dir = !sort_dir;
            else
                dir = sort_dir;
        }
        else // col-wise, same directions
        {
            row_wise = 0;
            next = next_col;
            prev = prev_col;
            dir = !sort_dir;
        }

        oddEvenSort(&val, cart_rank, next, prev, dir, dim[0], num_of_cuboids, row_wise, MPI_CUBOID_TYPE, comm);
        MPI_Barrier(comm);
        
        // //DEBUG
        // MPI_Gather(&val, 1, MPI_CUBOID_TYPE, cuboids, 1, MPI_CUBOID_TYPE, 0, comm);
        // if (rank == 0)
        // {
        //     printf("phase %d:\n", i);
        //     print_arr(cuboids, num_of_cuboids, dim[0]);
        //     printf("\n");
        // }
        // //DEBUG
    }

    MPI_Gather(&val, 1, MPI_CUBOID_TYPE, cuboids, 1, MPI_CUBOID_TYPE, 0, comm);

    if (rank == 0)
    {
        printf("after:\n");
        print_arr(cuboids, num_of_cuboids, dim[0]);
        printf("\n");
    }

    free(cuboids);
    MPI_Finalize(); //close mpi
    return 0;
}