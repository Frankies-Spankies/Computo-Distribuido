#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#define TAG_MESSAGE 1
int main(int argc, char** argv)
{
	int size;
	int rank;
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	
	int send_data;
	int recv_data;
	MPI_Request send_request;
	MPI_Request recv_request;
	send_data = 1000 + rank;
	MPI_Isend(&send_data, 1, MPI_INT, (rank + 1) % size, TAG_MESSAGE, MPI_COMM_WORLD, &send_request);
	MPI_Irecv(&recv_data, 1, MPI_INT, (rank + size - 1) % size, TAG_MESSAGE, MPI_COMM_WORLD, &recv_request);
	MPI_Waitall(1, &recv_request, MPI_STATUSES_IGNORE);
	printf("%5i recibio mensaje %i\n", rank, recv_data);
	MPI_Finalize();
	return 0;
}