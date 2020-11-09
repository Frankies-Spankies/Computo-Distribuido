#include <stdio.h>
#include <mpi.h>
#define TAG_MESSAGE 1

int main(int argc, char** argv){
	int size;
	int rank;
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	
	int* send_data = new int[size];
	int* recv_data = new int[size];
	
	MPI_Request* send_request = new MPI_Request[size];
	MPI_Request* recv_request =new MPI_Request[size];
	
	for (int r = 0; r < size; ++r){
		if (r != rank){
			send_data[r] = rank*1000 + r;
			MPI_Isend(&send_data[r], 1, MPI_INT, r, TAG_MESSAGE, MPI_COMM_WORLD, &send_request[r]);
			MPI_Irecv(&recv_data[r], 1, MPI_INT, r, TAG_MESSAGE, MPI_COMM_WORLD, &recv_request[r]);
		}else{
			send_request[r] = MPI_REQUEST_NULL;
			recv_request[r] = MPI_REQUEST_NULL;
		}
	}
	
	int received = 0;
	do{
		int r;
		MPI_Waitany(size, recv_request, &r, MPI_STATUS_IGNORE);
		printf("%i recibio mensaje '%i' desde %i\n", rank, recv_data[r], r);
		++received;
		} while (received < size - 1);
	delete[] recv_request;
	delete[] send_request;
	delete[] recv_data;
	delete[] send_data;
	MPI_Finalize();
	return 0;
}