#include <mpi.h>
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char *argv[])
{

  MPI_Init(&argc, &argv);
  int rank;
  int size;
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  /*CODE*/

  MPI_Finalize();
  return EXIT_SUCCESS;
}