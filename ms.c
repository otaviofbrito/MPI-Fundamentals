#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define LIMITE_SUP 2000
#define LIMITE_INF 1000
#define VALOR_SUP 99
#define VALOR_INF 0

void randomly_fill_array(int *nums, int size)
{
  srand(time(NULL));
  for (int i = 0; i < size; i++)
  {
    nums[i] = VALOR_INF + rand() % (VALOR_SUP - VALOR_INF + 1);
  }
}

int main(int argc, char **argv)
{
  // INIT
  MPI_Init(&argc, &argv);

  int num_procs;
  int rank;

  MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  // mestre
  if (rank == 0)
  {
    srand(time(NULL));
    int number_amount;
    int *numbers;
    // Send Messages
    for (int i = 1; i < num_procs; i++)
    {
      // 1. Gera um vetor aleatoria de tamanho entre LIMITE_SUP e LIMITE_INF
      number_amount = LIMITE_INF + rand() % (LIMITE_SUP - LIMITE_INF + 1);
      numbers = (int *)malloc(number_amount * sizeof(int));
      // 2. preencher o vetor
      randomly_fill_array(numbers, number_amount);
      // 3. enviar o  tamanho do vetor
      MPI_Send(&number_amount, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
      // 4. enviar o vetor
      MPI_Send(numbers, number_amount, MPI_INT, i, 0, MPI_COMM_WORLD);
      // 5. liberar o vetor
      free(numbers);
    }

    int total = 0;
    // Receive messages
    for (int i = 1; i < num_procs; i++)
    {
      int recv;
      MPI_Recv(&recv, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      total += recv;
      printf("\nReceveid sum: %d from process: %d\n", recv, i);
    }

    printf("\nTotal sum of all numbers: %d\n", total);
  }
  else
  {
    MPI_Status status;
    // receber tamanho do buffer
    int number_amount;
    MPI_Recv(&number_amount, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);

    // alocar buffer
    int *numbers = (int *)malloc(number_amount * sizeof(int));
    MPI_Recv(numbers, number_amount, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);

    int sum = 0;
    for (int i = 0; i < number_amount; i++)
    {
      sum += numbers[i];
    }

    MPI_Send(&sum, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
    free(numbers);
  }

  MPI_Barrier(MPI_COMM_WORLD);
  MPI_Finalize();
}
