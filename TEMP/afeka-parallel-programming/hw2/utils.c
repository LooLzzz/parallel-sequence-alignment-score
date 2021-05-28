#include "utils.h"

void oddEvenSort(CUBOID *val, int pivot, int next, int prev, DIR dir, int len, int num_of_cuboids, int row_wise, MPI_Datatype val_mpi_type, MPI_Comm comm)
{
    // int last;
    int i;
    CUBOID a, b;
    MPI_Status status;

    // last = num_of_cuboids - 1;
    a = *val;

    // DEBUG
    // if (j==1 && (pivot==0 || pivot==4 || pivot==8 || pivot==12))
    //     printf("> {rank=%2d, phase=%d} > before > val = %.2f\n", pivot, j, a.area);

    for (i = 0; i < len; i++)
    {
        if ((row_wise==1 && pivot%2 == 0) || (row_wise==0 && (pivot/len)%2 == 0))
        {
            if (i % 2 == 0)
            {
                MPI_Send(&a, 1, val_mpi_type, next, 0, comm);
                MPI_Recv(&b, 1, val_mpi_type, next, 0, comm, &status);
                a = minmax(a, b, dir);
            }
            else if ((row_wise==1 && pivot%len != 0 && pivot%len != len-1) || (row_wise==0 && pivot>=len && pivot<num_of_cuboids-len))
            {
                MPI_Send(&a, 1, val_mpi_type, prev, 0, comm);
                MPI_Recv(&b, 1, val_mpi_type, prev, 0, comm, &status);
                a = minmax(a, b, !dir);
            }
        }
        else
        {
            if (i % 2 == 0)
            {
                MPI_Send(&a, 1, val_mpi_type, prev, 0, comm);
                MPI_Recv(&b, 1, val_mpi_type, prev, 0, comm, &status);
                a = minmax(a, b, !dir);
            }
            else if ((row_wise==1 && pivot%len != 0 && pivot%len != len-1) || (row_wise==0 && pivot>=len && pivot<num_of_cuboids-len))
            {
                MPI_Send(&a, 1, val_mpi_type, next, 0, comm);
                MPI_Recv(&b, 1, val_mpi_type, next, 0, comm, &status);
                a = minmax(a, b, dir);
            }
        }
    }

    *val = a;
    // return a;
}

void print_arr(CUBOID arr[], int len, int newline)
{
    // int s = sqrt(len);
    for (int i = 0; i < len; i++)
    {
        printf("%-5.1f  ", arr[i].area);
        if ((i+1)%newline==0)
            printf("\n");
    }
}

void print_float_arr(float arr[], int len)
{
    int s = sqrt(len);
    for (int i = 0; i < len; i++)
    {
        printf("%-5.1f  ", arr[i]);
        if ((i+1)%s==0)
            printf("\n");
    }
}

void get_cubes_from_file(char *file_path, int num_of_processes, int *num_of_cubiods, DIR *dir, CUBOID **cuboids)
{
    int i;
    char str[MAXCHAR];
    CUBOID *cube;
    FILE *file = fopen(file_path, "r");

    if (file == NULL)
    {
        fprintf(stderr, "Could not open file %s\n", file_path);
        MPI_Abort(MPI_COMM_WORLD, MPI_ERR_BAD_FILE);
    }

    // read size
    fscanf(file, "%d\n", num_of_cubiods);
    if (*num_of_cubiods != num_of_processes)
    {
        fprintf(stderr, "Number of cubiods doesn't equal number of processes!\n");
        MPI_Abort(MPI_COMM_WORLD, MPI_ERR_BAD_FILE);
    }
    
    // read direction
    fscanf(file, "%d\n", dir);

    // read values
    *cuboids = malloc(sizeof(CUBOID) * (*num_of_cubiods));
    i = 0;
    while (fgets(str, MAXCHAR, file) != NULL)
    {
        cube = &((*cuboids)[i++]);
        sscanf(str, "%d %f %f %f", &cube->id, &cube->h, &cube->w, &cube->d);
        
        cube->area = 
              (cube->h * cube->w) * 2
            + (cube->h * cube->d) * 2
            + (cube->w * cube->d) * 2;
        
        // printf("%-3d %-4.1f %-4.1f %-4.1f -> %.1f\n", cube->id, cube->h, cube->w, cube->d, cube->area);
    }

    fclose(file);
}

MPI_Datatype COMMIT_MPI_CUBOID_TYPE()
{
    int nitems = 5;
    int blocklengths[] = {1,1,1,1,1};
    MPI_Datatype types[] = {MPI_INT, MPI_FLOAT, MPI_FLOAT, MPI_FLOAT, MPI_FLOAT};
    MPI_Datatype MPI_CUBOID_TYPE;
    MPI_Aint offsets[5];

    offsets[0] = offsetof(CUBOID, id);
    offsets[1] = offsetof(CUBOID, h);
    offsets[2] = offsetof(CUBOID, w);
    offsets[3] = offsetof(CUBOID, d);
    offsets[4] = offsetof(CUBOID, area);

    MPI_Type_create_struct(nitems, blocklengths, offsets, types, &MPI_CUBOID_TYPE);
    MPI_Type_commit(&MPI_CUBOID_TYPE);

    return MPI_CUBOID_TYPE;
}