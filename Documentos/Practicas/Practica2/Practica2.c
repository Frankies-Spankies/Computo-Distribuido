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
int module = -1;

//Implementacion ============>
void inicializa()
{
    time_t t;
    srand(time(&t));
}

void initiliaze_slave()
{
    MPI_Recv(&slice_size, 1, MPI_INT, 0, SLICE_SIZE, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    arr_wanted = (int *)malloc((slice_size) * sizeof(int));
    arr_index = (int *)malloc((slice_size) * sizeof(int));
    for (int i = 0; i < slice_size; i++)
    {
        arr_index[i] = -1;
    } // inicializa todo arr_index a -1
}

void initiliaze_master(char **argv)
{
    leng = (*argv[1] - '0');
    array = (int *)malloc((leng) * sizeof(int));
    index_wanted = rand() % leng;
    module = (leng) % (size - 1);

    if (module != 0)
    {
        indx = module;
    }

    slice_size = (leng - module) / (size - 1);

    

    arr_wanted =(int *)malloc((slice_size) * sizeof(int));

    int arr_result[leng][slice_size]; // TODO PONER EN OTRA PARTE

    for (int f = 0; f < leng; f++)
    {
        for (int c = 0; c < slice_size; c++)
        {
            arr_result[f][c] = -1;
        }
    }


    for (int  i = 1; i < size; i++)
    {
        MPI_Send(&slice_size, 1, MPI_INT, i, SLICE_SIZE, MPI_COMM_WORLD);
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

void fill_arr_wanted()
{
    for (int j = 0; j < slice_size; j++)
    {
        arr_wanted[j] = array[indx+j];
    }
}

void send_data_to_search()
{
    for (int i = 1; i < size; i++)
    {
        fill_arr_wanted();
        MPI_Send(&indx, 1, MPI_INT, i, INDEX, MPI_COMM_WORLD);
        MPI_Send(&arr_wanted, slice_size, MPI_INT, i, SLICE, MPI_COMM_WORLD);
        indx += slice_size;
    }
    //Busqueda individual en con residuos en master
    for (int l = 0; l < module; l++)
    {
        arr_wanted[l] = array[l];
    }
    indx = 0;
    slice_size = module;
}

void recive_data_to_search()
{
    MPI_Recv(&indx, 1, MPI_INT, 0, INDEX, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    MPI_Recv(&arr_wanted, slice_size, MPI_INT, 0, SLICE, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    //printf("algo: %d", arr_wanted[0]);
}

int distributed_search(int argc, char **argv)
{
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    data_search();
    send_data_to_search();

    MPI_Finalize();
    return 0;
}

//Tests ===============>

void test_initiliaze_master(char **argv)
{
    if (rank == 0)
    {
        printf("\n####test_initiliaze_master####\n");
        //when
        initiliaze_master(argv);
        //then
        printf("\nsize: %d", size);
        printf("\nleng: %d", leng);
        printf("\nindex_wanted: %d", index_wanted);
        printf("\nslice_size: %d", slice_size);
        printf("\nmodule: %d", module);
        printf("\nindx: %d\n", indx);
    }
}

void test_data_search(char **argv)
{
    if (rank == 0)
    {
        initiliaze_master(argv);
        data_search();

        printf("\nArray ");
        for (int i = 0; i < leng; i++)
        {
            printf(" %d", array[i]);
        }
        printf("\nwanted: %d, indx: %d, slice_size: %d\n", wanted, indx, slice_size);
    }
}

void test_send_recive_data_to_search(char **argv)
{
    if (rank == 0)
    {

        initiliaze_master(argv);
        printf("Enviando slice_size");
    }else
    {
        initiliaze_slave();
        printf("Reciviendo slice_size");
    }
    
    
    if (rank == 0)
    {
        //given
        data_search();
        send_data_to_search();
/*         printf("\nnodo: %d\n", rank);
        printf("\nindx: %d\n", indx);
        printf("\nslice_size: %d\n", slice_size); */
    }
    else
    {
        //given
        recive_data_to_search();
        //then
        printf("\nnodo: %d\n", rank);
        printf("\nindx: %d\n", indx);
        printf("\nslice_size: %d\n", slice_size);
        printf("\n=====>Arr_wanted ");
        printf("algo: %d", arr_wanted[1]);
        for (int i = 0; i < slice_size; i++)
        {
            //printf(" %d", arr_wanted[i]);
        }
    }
}

int test(int argc, char **argv)
{
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    //inicia prueba
    test_send_recive_data_to_search(argv);
    //fun de prueba

    MPI_Finalize();
    return 0;
}

/**
 * Funcion principal 
*/
int main(int argc, char **argv)
{
    inicializa();
    test(argc, argv);
}