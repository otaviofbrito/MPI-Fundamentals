#include <mpi.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

int main(int argc, char *argv[])
{

  MPI_Init(&argc, &argv);
  int rank;
  int size;
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  srand(time(NULL) + rank);
  // Generate random value for each process
  int process_value = rand() % 100;
  printf("Process %d generate value %d \n", rank, process_value);
  int total_sum;
  // Reduce
  MPI_Reduce(&process_value, &total_sum, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

  if (rank == 0)
  {
    printf("Total sum = %d\n", total_sum);
  }

  MPI_Finalize();
  return EXIT_SUCCESS;
}