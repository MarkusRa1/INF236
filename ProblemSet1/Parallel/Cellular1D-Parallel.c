#include <mpi.h> // Header defining MPI functions
#include <stdio.h>
#include <string.h>
int main(int argc, char **argv)
{
    int comm_sz = 10;            /* Number of processes */
    int my_rank = 0;            /* My process rank */
    MPI_Init(&argc, &argv); // No MPI function before this call
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    if (my_rank != 0)
    {
        printf("rank %d", my_rank);
    }
    else
    {
        printf("rank 0");
    }
    // Code
    MPI_Finalize(); // No MPI function after this call
    return 0;
}