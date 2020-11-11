#include <stdio.h>
#include <mpi.h>

#define TAG_VUELTAUNO 1
#define TAG_VUELTADOS 2

int main(int argc, char** argv)
{
    // Inicio OpenMPI
	int size, rank;
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    //Ejecutar
    int eleccion=1;

    for (int r = 0; r <= &rank; r++)
    {
    	MPI_Send(&eleccion, 1, MPI_INT, r, TAG_VUELTAUNO, MPI_COMM_WORLD);
    }
    

    //Fin de ejecucion 
	MPI_Finalize();
	return 0;
    
}