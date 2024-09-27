#include <mpi.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#define MAX_FLOAT 10.0

void randomize_matrix(float *matrix, int rows, int cols)
{
  for (int i = 0; i < rows; i++)
  {
    for (int j = 0; j < cols; j++)
      matrix[i * cols + j] = ((float)rand() / (float)RAND_MAX) * MAX_FLOAT;
  }
}

void sum_matrix(float *A, float *B, float *C, int dim)
{
  for (int i = 0; i < dim; i++)
    C[i] = A[i] + B[i];
}

void print_matrix(float *matrix, int rows, int cols)
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

int main(int argc, char *argv[])
{
  if (argc != 3)
  {
    printf("Usage: num_rows num_columns");
    return EXIT_FAILURE;
  }
  int rows = atoi(argv[1]);
  int cols = atoi(argv[2]);
  int dim = rows * cols;

  MPI_Init(&argc, &argv);
  int rank;
  int size;
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  srand(time(NULL) + rank);

  if (rows % size != 0)
  {
    if (rank == 0)
    {
      printf("Number of processes must be compatible with the number of rows!\n");
    }
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Finalize();
    return EXIT_FAILURE;
  }

  float *matrix_A = NULL;
  float *matrix_B = NULL;
  float *matrix_C = NULL;

  if (rank == 0)
  {
    matrix_A = (float *)malloc(dim * sizeof(float));
    matrix_B = (float *)malloc(dim * sizeof(float));
    matrix_C = (float *)malloc(dim * sizeof(float));
    randomize_matrix(matrix_A, rows, cols);
    randomize_matrix(matrix_B, rows, cols);
  }

  int chunk_size = (rows / size) * cols;

  float *portion_A = (float *)malloc(chunk_size * sizeof(float));
  float *portion_B = (float *)malloc(chunk_size * sizeof(float));

  MPI_Scatter(matrix_A, chunk_size, MPI_INT, portion_A, chunk_size, MPI_INT, 0, MPI_COMM_WORLD);
  MPI_Scatter(matrix_B, chunk_size, MPI_INT, portion_B, chunk_size, MPI_INT, 0, MPI_COMM_WORLD);

  float *portion_C = (float *)malloc(chunk_size * sizeof(float));
  sum_matrix(portion_A, portion_B, portion_C, chunk_size);

  MPI_Gather(portion_C, chunk_size, MPI_INT, matrix_C, chunk_size, MPI_INT, 0, MPI_COMM_WORLD);

  if (rank == 0)
  {
    printf("\n >Number of process: %d \n >%d rows/process \n >%d elements/process of %d total\n\n",
           size, chunk_size / cols, chunk_size, dim);
    printf("MATRIX A: \n");
    printf("MATRIX A: \n");
    print_matrix(matrix_A, rows, cols);
    printf("\nMATRIX B: \n");
    print_matrix(matrix_B, rows, cols);
    printf("\nMATRIX C: \n");
    print_matrix(matrix_C, rows, cols);
    free(matrix_A);
    free(matrix_B);
    free(matrix_C);
  }

  free(portion_A);
  free(portion_B);
  free(portion_C);

  MPI_Barrier(MPI_COMM_WORLD);
  MPI_Finalize();
  return EXIT_SUCCESS;
}