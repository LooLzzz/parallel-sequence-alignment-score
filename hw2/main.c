#include "main.h"

int main(int argc, char *argv[])
{
    // char file_path[MAXCHAR] = argv[1];
    int num_of_processes, rank, num_of_cuboids, i, max_phases, next, prev;
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
        print_arr(cuboids, num_of_cuboids);
        printf("\n");
    }
    
    // pass dim[] & sort_dir to all processes
    MPI_Bcast(dim, 2, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&sort_dir, 1, MPI_INT, 0, MPI_COMM_WORLD);
    // num_of_cuboids = dim[0]*dim[1];

    // create cartesian coords system
    MPI_Cart_create(MPI_COMM_WORLD, 2, dim, period, reorder, &comm);
    MPI_Cart_coords(comm, rank, 2, coords);
    MPI_Cart_rank(comm, coords, &cart_rank);
    MPI_Cart_shift(comm, 0, 1, &prev_col, &next_col); //shift cols
    MPI_Cart_shift(comm, 1, 1, &prev_row, &next_row); //shift rows
    // printf("{rank=%2d, cart_rank=%2d}: coords=(%d,%d), next=(%2d,%2d), prev=(%2d,%2d)\n", rank, cart_rank, coords[0], coords[1], next_row, next_col, prev_row, prev_col);

    MPI_Scatter(cuboids, 1, MPI_CUBOID_TYPE, &val, 1, MPI_CUBOID_TYPE, 0, comm);
    // printf("{rank=%d, id=%d}: %.1f x %.1f x %.1f\n", rank, val.id, val.w, val.h, val.d);

    // (2log(n)+1) row/column phases are need to sort n^2 cuboids
    max_phases = (int) (2*log2(dim[0])) + 1;
    for (i = 0; i < max_phases; i++)
    {
        if (i % 2 == 0) // row-wise, snake-like directions
        {
            next = next_row;
            prev = prev_row;
            if (coords[0] % 2 == 0)
                dir = sort_dir;
            else
                dir = !sort_dir;
        }
        else // col-wise, same directions
        {
            next = next_col;
            prev = prev_col;
            dir = sort_dir;
        }

        printf("> {phase %d} > rank[%2d] = coords[%d,%d] = %5.1f > next=%2d, prev=%2d, dir=%d\n", i, cart_rank, coords[0], coords[1], val.area, next, prev, dir);
        // printf("> {coords=(%d,%d), phase=%d} > val_before = %.1f\n", coords[0], coords[1], i, val.area);
        // printf("> {rank=%2d, phase=%d} > val_before = %.1f\n", cart_rank, i, val.area);
        // printf("> {rank=%2d} > before barrier\n", cart_rank);
        
        oddEvenSort(&val, cart_rank, next, prev, dir, dim[0], num_of_cuboids, MPI_CUBOID_TYPE, comm);
        MPI_Barrier(comm);
        
        // printf("> {rank=%2d} > after barrier\n", cart_rank);
        // printf("> {rank=%2d, phase=%d} > val_after  = %.1f\n", cart_rank, i, val.area);
        // printf("> {coords=(%d,%d), phase=%d} > val_after  = %.1f\n", coords[0], coords[1], i, val.area);
        
        //DEBUG
        MPI_Gather(&val, 1, MPI_CUBOID_TYPE, cuboids, 1, MPI_CUBOID_TYPE, 0, comm);
        if (rank == 0)
        {
            printf("phase %d:\n", i);
            print_arr(cuboids, num_of_cuboids);
            printf("\n");
        }
        //DEBUG
    }

    MPI_Gather(&val, 1, MPI_CUBOID_TYPE, cuboids, 1, MPI_CUBOID_TYPE, 0, comm);

    if (rank == 0)
    {
        printf("after:\n");
        print_arr(cuboids, num_of_cuboids);
        printf("\n");
    }

    MPI_Finalize(); //close mpi
    return 0;
}