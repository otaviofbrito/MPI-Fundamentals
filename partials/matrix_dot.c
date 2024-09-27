#include <mpi.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#define MAX_VALUE 1000.0

void print_matrix(double *matrix, int rows, int cols)
{
  for (int i = 0; i < rows; i++)
  {
    for (int j = 0; j < cols; j++)
    {
      printf(" %f ", matrix[i * cols + j]);
    }
    printf("\n");
  }
}

void random_fill_matrix(double *matrix, int rows, int cols)
{
  for (int i = 0; i < rows; i++)
  {
    for (int j = 0; j < cols; j++)
    {
      matrix[i * cols + j] = ((float)rand() / (float)RAND_MAX) * MAX_VALUE;
    }
  }
}

void dot_product(double *A, double *B, double *C, int colsA, int colsB)
{
  for (int j = 0; j < colsB; j++)
  {
    C[j] = 0;
    for (int k = 0; k < colsA; k++)
    {
      C[j] += A[k] * B[k * colsB + j];
    }
  }
}

int main(int argc, char *argv[])
{
  if (argc != 2)
  {
    printf("Usage: %s num_colsA_rowsB\n", argv[0]);
    return EXIT_FAILURE;
  }

  // A = X*Y ; B = W*Z ; Y == W == input parameter
  // X == Z == number of processes
  // Number of Columns in Matrix A == Number of Rows in Matrix B
  // Number of Rows in Matrix A == Number of Columns in Matrix B == Number of Processes
  // Each process receives a row of A and all columns of B to perform the multiplication

  MPI_Init(&argc, &argv);

  // Number of Columns in Matrix A == Number of Rows in Matrix B
  int colsA, rowsB;
  colsA = rowsB = atoi(argv[1]);

  int rank;
  int size;
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  // Number of Rows in Matrix A == Number of Columns in Matrix B == Number of Processes
  int rowsA, colsB;
  rowsA = colsB = size;

  srand(time(NULL) + rank);

  double *matrix_A = NULL;
  double *matrix_B = (double *)malloc((rowsB * colsB) * sizeof(double));
  double *matrix_C = NULL;

  if (rank == 0)
  {
    matrix_A = (double *)malloc((rowsA * colsA) * sizeof(double));
    matrix_C = (double *)malloc((rowsA * colsB) * sizeof(double));
    random_fill_matrix(matrix_A, rowsA, colsA);
    random_fill_matrix(matrix_B, rowsB, colsB);
  }

  int chunk_size_a = colsA;
  int chunk_size_c = colsB;
  double *local_A = (double *)malloc(chunk_size_a * sizeof(double));
  double *local_C = (double *)malloc(chunk_size_c * sizeof(double));

  // Distribute the rows of matrix_A
  MPI_Scatter(matrix_A, chunk_size_a, MPI_DOUBLE, local_A, chunk_size_a, MPI_DOUBLE, 0, MPI_COMM_WORLD);

  // Broadcast matrix_B to all processes
  MPI_Bcast(matrix_B, (rowsB * colsB), MPI_DOUBLE, 0, MPI_COMM_WORLD);

  // Calculate the dot product of a row of A with B
  dot_product(local_A, matrix_B, local_C, colsA, colsB);

  // Gather the results from all processes
  MPI_Gather(local_C, chunk_size_c, MPI_DOUBLE, matrix_C, chunk_size_c, MPI_DOUBLE, 0, MPI_COMM_WORLD);

  if (rank == 0)
  {
    printf("\nMATRIX A = \n");
    print_matrix(matrix_A, size, colsA);
    printf("\nMATRIX B = \n");
    print_matrix(matrix_B, rowsB, size);
    printf("\nMATRIX C (AxB) = \n");
    print_matrix(matrix_C, rowsA, colsB);
    free(matrix_A);
    free(matrix_C);
  }

  free(matrix_B);
  free(local_A);
  free(local_C);

  MPI_Finalize();
  return EXIT_SUCCESS;
}
