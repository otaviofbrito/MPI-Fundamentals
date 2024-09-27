#include <mpi.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

void print_matrix(int *matrix, int rows, int cols)
{
  for (int i = 0; i < rows; i++)
  {
    for (int j = 0; j < cols; j++)
    {
      printf(" %d ", matrix[i * cols + j]);
    }
    printf("\n");
  }
}

void random_fill_matrix(int *matrix, int rows, int cols)
{
  for (int i = 0; i < rows; i++)
  {
    for (int j = 0; j < cols; j++)
    {
      matrix[i * cols + j] = rand() % 500;
    }
  }
}

void dot_product(int *A, int *B, int *C, int rowsA, int colsA, int rowsB, int colsB)
{
  if (colsA != rowsB)
  {
    printf("Matrix multiplication not possible!\n");
    return;
  }

  // Multiply matrices A and B and store the result in C
  for (int i = 0; i < rowsA; i++)
  {
    for (int j = 0; j < colsB; j++)
    {
      C[i * colsB + j] = 0;
      for (int k = 0; k < colsA; k++)
      {
        C[i * colsB + j] += A[i * colsA + k] * B[k * colsB + j];
      }
    }
  }
}

int main(int argc, char *argv[])
{
  if (argc != 5)
  {
    printf("Usage: ./matrix num_rows_A num_cols_A num_rows_B num_cols_B");
    return EXIT_FAILURE;
  }

  int rowsA = atoi(argv[1]);
  int colsA = atoi(argv[2]);
  int rowsB = atoi(argv[3]);
  int colsB = atoi(argv[4]);

  if (colsA != rowsB)
  {
    printf("Matrix multiplication not possible!\n");
    return EXIT_FAILURE;
  }

  MPI_Init(&argc, &argv);
  int rank;
  int size;
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  if (rowsA % size != 0)
  {
    printf("Number of process must be equal of number of rows A!\n");
    return MPI_Finalize();
  }

  srand(time(NULL) + rank);

  int *matrix_A = (int *)malloc((rowsA * colsA) * sizeof(int));
  int *matrix_B = (int *)malloc((rowsB * colsB) * sizeof(int));

  if (rank == 0)
  {
    random_fill_matrix(matrix_A, rowsA, colsA);
    random_fill_matrix(matrix_B, rowsB, colsB);
  }

  int chunk_size = (rowsA / size) * colsA;
  int *local_A = (int *)malloc(chunk_size * sizeof(int));
  // Send data chunks for each process
  MPI_Scatter(matrix_A, chunk_size, MPI_INT, local_A, chunk_size, MPI_INT, 0, MPI_COMM_WORLD);

  // Send Matrix B to all other process;
  MPI_Bcast(matrix_B, (rowsB * colsB), MPI_INT, 0, MPI_COMM_WORLD);

  // Computs dot product in parallel
  int chunk_size_c = (rowsA / size) * colsB;
  int *local_C = (int *)malloc(chunk_size_c * sizeof(int));
  dot_product(local_A, matrix_B, local_C, chunk_size / colsA, colsA, rowsB, colsB);

  // Gather data from all process
  int *matrix_C = (int *)malloc((rowsA * colsB) * sizeof(int));
  MPI_Gather(local_C, chunk_size_c, MPI_INT, matrix_C, chunk_size_c, MPI_INT, 0, MPI_COMM_WORLD);

  if (rank == 0)
  {
    printf("MATRIX A: \n");
    print_matrix(matrix_A, rowsA, colsA);
    printf("MATRIX B: \n");
    print_matrix(matrix_B, rowsB, colsB);
    printf("MATRIX C: \n");
    print_matrix(matrix_C, rowsA, colsB);
  }

  free(matrix_A);
  free(matrix_B);
  free(matrix_C);
  free(local_A);
  free(local_C);

  MPI_Finalize();
  return EXIT_SUCCESS;
}