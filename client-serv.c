
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define LIMITE_SUP 2000000
#define LIMITE_INF 1000000
#define VALOR_SUP 99
#define VALOR_INF 0

void randomly_fill_array(int *nums, int size)
{
  for (int i = 0; i < size; i++)
  {
    nums[i] = VALOR_INF + rand() % (VALOR_SUP - VALOR_INF + 1);
  }
}

int main(int argc, char **argv)
{
  // Init
  MPI_Init(&argc, &argv);

  int num_procs;
  int rank;
  MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  const int M = 5 * num_procs;
  int nums_env = 0;
  int nums_recv = 0;

  // mestre
  if (rank == 0)
  {
    srand(time(NULL));
    int total = 0;
    while (nums_recv < M)
    {
      MPI_Status status;
      if (nums_env < M)
      {
        // 0. Receber notificacao do escravo;
        int flag;
        MPI_Recv(&flag, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
        // 1. Gera um vetor aleatorio de tamanho entre LIMITE_SUP e LIMITE_INF
        int number_amount = LIMITE_INF + rand() % (LIMITE_SUP - LIMITE_INF + 1);
        int *numbers = (int *)malloc(number_amount * sizeof(int));
        // 2. preencher o vetor
        randomly_fill_array(numbers, number_amount);
        // 3. enviar o  tamanho do vetor
        MPI_Send(&number_amount, 1, MPI_INT, status.MPI_SOURCE, 0, MPI_COMM_WORLD);
        // 4. enviar o vetor
        MPI_Send(numbers, number_amount, MPI_INT, status.MPI_SOURCE, 0, MPI_COMM_WORLD);
        printf("\nEnviou %d numeros para o processo %d", number_amount, status.MPI_SOURCE);
        // 5. liberar o vetor
        free(numbers);
        nums_env++;
      }

      int val;
      MPI_Recv(&val, 1, MPI_INT, MPI_ANY_SOURCE, 1, MPI_COMM_WORLD, &status);
      total += val;
      nums_recv++;
      printf("\n%d: Recebeu soma: %d do processo: %d\n", nums_recv, val, status.MPI_SOURCE);
    }

    printf("\nSoma total: %d\n", total);
    MPI_Abort(MPI_COMM_WORLD, 1);
  }
  else
  {
    while (1)
    {
      // Notificar o mestre que está disponível
      MPI_Send(&rank, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
      printf("\nProcesso %d esta disponivel!", rank);

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

      MPI_Send(&sum, 1, MPI_INT, 0, 1, MPI_COMM_WORLD);
      free(numbers);
    }
  }

  MPI_Barrier(MPI_COMM_WORLD);
  MPI_Finalize();
}
