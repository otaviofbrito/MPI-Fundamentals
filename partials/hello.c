#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MSG_LENGTH 100

int main(int argc, char **argv)
{
  MPI_Init(&argc, &argv);

  int num_procs;
  int proc_id;
  MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
  MPI_Comm_rank(MPI_COMM_WORLD, &proc_id);

  char msg[MSG_LENGTH];
  sprintf(msg, "Hello mpi from process: %d, of %d total!", proc_id, num_procs);

  // Send msg to each other process
  for (int i = 0; i < num_procs; i++)
  {
    if (proc_id != i)
    {
      MPI_Send(msg, MSG_LENGTH, MPI_CHAR, i, 0, MPI_COMM_WORLD);
    }
  }

  // Receive msgs
  char *recv_msgs[num_procs];

  for (int i = 0; i < num_procs; i++)
  {
    if (proc_id != i)
    {
      recv_msgs[i] = (char *)malloc(MSG_LENGTH * sizeof(char));
      MPI_Recv(recv_msgs[i], MSG_LENGTH, MPI_CHAR, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }
  }

  printf("\nProcess %d received following msgs: \n", proc_id);
  for (int i = 0; i < num_procs; i++)
  {
    if (proc_id != i)
    {
      printf("Process [%d]: %s\n", i, recv_msgs[i]);
      free(recv_msgs[i]);
    }
  }

  MPI_Finalize();
  return EXIT_SUCCESS;
}