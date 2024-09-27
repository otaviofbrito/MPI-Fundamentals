#include <mpi.h>
#include <stdlib.h>
#include <stdio.h>

#define MSG_LENGTH 100

int main(int argc, char *argv[])
{

  MPI_Init(&argc, &argv);

  int num_procs;
  int proc_id;
  MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
  MPI_Comm_rank(MPI_COMM_WORLD, &proc_id);

  char *msg = (char *)malloc(MSG_LENGTH * sizeof(char));
  if (proc_id == 0)
  {
    sprintf(msg, "Message from process [%d]\n", proc_id);
  }

  MPI_Bcast(msg, MSG_LENGTH, MPI_CHAR, 0, MPI_COMM_WORLD);
  printf("Process %d receveid following data: %s", proc_id, msg);

  free(msg);
  MPI_Finalize();
  return EXIT_SUCCESS;
}