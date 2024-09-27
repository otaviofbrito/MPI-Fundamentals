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

  printf("Process %d before barrier\n", rank);

  MPI_Barrier(MPI_COMM_WORLD);

  printf("Process %d after barrier\n", rank);

  MPI_Finalize();
  return EXIT_SUCCESS;
}