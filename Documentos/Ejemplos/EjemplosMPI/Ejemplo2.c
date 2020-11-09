#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define TAG_LENGTH 1
#define TAG_DATA 2

int main(int argc, char** argv)
{
	int size;
	int rank;
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	if (rank == 0) // Maestro
	{
		printf("MASTER (size = %i)\n", size);
		int length = 20;
		double* data = (double*)malloc(sizeof(double)*length);
		for (int r = 1; r < size; ++r)
		{
			MPI_Send(&length, 1, MPI_INT, r, TAG_LENGTH, MPI_COMM_WORLD);
			MPI_Send(data, length, MPI_DOUBLE, r, TAG_DATA, MPI_COMM_WORLD);
		}free(data);
	}else // Esclavos
	{
		int length;
		printf("SLAVE (rank = %i)\n", rank);
		MPI_Recv(&length, 1, MPI_INT, 0, TAG_LENGTH, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
				printf("SLAVE (rank = %i) recibido\n", rank);
		double* data = (double*)malloc(sizeof(double)*length);
		MPI_Recv(data, length, MPI_DOUBLE, 0, TAG_DATA, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

		free(data);
	}
	MPI_Finalize();
	return 0;
}