#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <time.h>

#define SLICE 1
#define INDEX 2
#define SEARCH_RESULT 3
#define WANTED 4
#define SLICE_SIZE 5

int size, rank;

//comunes
int wanted;
int *arr_wanted;
int indx;
int slice_size;
int *arr_index;

//nodo maestro
int leng;
int *array;
int index_wanted;

void inicializa()
{
    time_t t;
    srand(time(&t));
}

fill_arr_wanted()
{
    for (size_t i = 0; i < count; i++)
    {
        /* code */
    }
}

void data_search()
{
    for (int i = 0; i < leng; i++)
    {
        int new = rand() % leng;
        if (i == index_wanted)
        {
            wanted = new;
        }
        array[i] = new;
    }
}

void test_data_search()
{
    data_search();
    char array_cadena[leng];

    for (int i = 0; i < leng; i++)
    {
        array_cadena[i] = array[i] + '0';
    }
    printf("\narray: [%s], wanted: %d\n", array_cadena, wanted);
}

void send_data_to_search()
{
    for (int i = indx; i <= indx + slice_size; i++)
    {
        arr_wanted[i] = array[i];
    }
    
    MPI_Send(&indx, 1, MPI_INT, i, INDEX, MPI_COMM_WORLD); 
    MPI_Send(&slice_size, 1, MPI_INT, i, SLICE_SIZE, MPI_COMM_WORLD); 
    MPI_Send(&arr_wanted, slice_size, MPI_INT, i, SLICE, MPI_COMM_WORLD); 
    indx+=slice_size;
}

void recive_data_to_search(){
    MPI_Recv(&indx, 1, MPI_INT, 0, INDEX, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    MPI_Recv(&slice_size, 1, MPI_INT, 0, SLICE_SIZE, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    arr_wanted=(int *)malloc((slice_size) * sizeof(int));
    arr_index=(int *)malloc((slice_size) * sizeof(int));
    for (int i = 0; i < slice_size; i++){arr_index[i]=-1;} // inicializa todo arr_index a -1
    MPI_Recv(&arr_wanted, slice_size, MPI_INT, 0, SLICE, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

}


int distributed_search(int argc, char **argv)
{
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (rank != 0)
    {
        wanted = -1;
        arr_wanted = NULL;
        indx = -1;
        slice_size = NULL;
        arr_index = NULL;
    }
    else
    {
        leng = (*argv[1] - '0');
        array = (int *)malloc((leng) * sizeof(int));
        index_wanted = rand() % leng;
        int arr_result[leng][slice_size];
        slice_size = leng / size;

        for (int f = 0; f < leng; f++)
        {
            for (int c = 0; c < slice_size; c++)
            {
                arr_result[f][c]=-1
            }
        }
        
    }

    if (rank == 0)
    {
        data_search();
        send_data_to_search();
    }


    MPI_Finalize();
    return 0;
}

int test(int argc, char **argv)
{
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    indx = 0;

    leng = (*argv[1] - '0');
    slice_size = leng / size;
    arr_index = (int *)malloc((slice_size) * sizeof(int));

    int arr_result[leng][slice_size];

    index_wanted = rand() % leng;
    array = (int *)malloc((leng) * sizeof(int));

    //Inicia prueba
    if (rank == 0)
    {
        test_data_search();
    }

    //Fin de prueba

    MPI_Finalize();
    return 0;
}

int main(int argc, char **argv)
{
    inicializa();
    test(argc, argv);
}