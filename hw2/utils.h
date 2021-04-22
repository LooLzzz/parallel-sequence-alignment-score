#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stddef.h>
#include <math.h>
#include <mpi.h>

typedef enum DIR {
    DEC,
    ASC
} DIR;

typedef struct CUBOID
{
    int id;
    float h, w, d, area;
} CUBOID;

#define MAXCHAR 20
#define RAND_ARR_MAX 100

// // FOR INT
// #define max(a,b) (a > b ? a : b)
// #define min(a,b) (a < b ? a : b)
// #define minmax(a,b,dir) (dir ? max(a,b) : min(a,b))

// FOR CUBOIDS
#define max_area(a,b)   (a.area > b.area ? a : b)
#define min_area(a,b)   (a.area < b.area ? a : b)
#define max_width(a,b)  (a.w > b.w ? a : b)
#define min_width(a,b)  (a.w < b.w ? a : b)
#define max(a,b)        (a.area == b.area ? max_width(a,b) : max_area(a,b) )
#define min(a,b)        (a.area == b.area ? min_width(a,b) : min_area(a,b) )
#define minmax(a,b,dir) (dir ? max(a,b) : min(a,b))

void oddEvenSort(CUBOID *val, int pivot, int next, int prev, DIR dir, int len, int num_of_cuboids, MPI_Datatype val_mpi_type, MPI_Comm comm);
// CUBOID minmax(CUBOID a, CUBOID b, DIR dir);

void print_arr(CUBOID arr[], int len);
void print_float_arr(float arr[], int len);
void get_cubes_from_file(char *file_path, int num_of_processes, int *num_of_cubiods, DIR *dir, CUBOID **cuboids);
MPI_Datatype COMMIT_MPI_CUBOID_TYPE();