/**
 * Practica 2
 * Francisco Daniel Gamez Garcia
*/

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
int global_slice_size;

//Implementacion ============>
void inicializa()
{
    time_t t;
    srand(time(&t));
}

/**
 * Recive el elemento a bucar 
  */
void scan_wanted()
{
    printf("Cual es el valor a buscar:");
    char buffer[100];
    fgets(buffer, sizeof buffer, stdin);
    sscanf(buffer, "%d", &wanted);
}

/**
 * inicializa cada elemento del arreglo arr_index a -1  
  */
void clean_arr_index()
{
    for (int i = 0; i < slice_size; i++)
    {
        arr_index[i] = -1;
    } // inicializa todo arr_index a -1
}

/**
 * Configura los valores iniciales para las variable arr_wanted, arr_index y slice_size, nesesarios para 
 * hacer la busqueda inidividual por nodos, en particular de los nodos esclavos
  */
void initiliaze_slave()
{
    MPI_Recv(&slice_size, 1, MPI_INT, 0, SLICE_SIZE, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    arr_wanted = (int *)malloc((slice_size) * sizeof(int));
    arr_index = (int *)malloc((slice_size) * sizeof(int));
    clean_arr_index();
}

/**
 * Configura los valores iniciales tales como la cantidad de elementos que le va tocar a cada nodo (slice_size), 
 * los elementos que le que va a tener que procesar en nodo maestro en caso de que haya residio en la division numero_nodos/numero_elemntos_del_arreglo_generado
 * Recopila los parametros pasados por entrada estandar
 * Envia los valores nescesarios para que los nodos esclavos puedan inicializarse sin nesesidad de tomar los valores de la entrada estandar
  */
void initiliaze_master(char **argv)
{
    leng = atoi(argv[1]);
    array = (int *)malloc((leng) * sizeof(int));
    index_wanted = rand() % leng;
    module = (leng) % (size - 1);

    if (module != 0)
    {
        indx = module;
    }

    slice_size = (leng - module) / (size - 1);
    arr_index = (int *)malloc((slice_size) * sizeof(int));
    clean_arr_index();

    arr_wanted = (int *)malloc((slice_size) * sizeof(int));

    //Envia slice_size
    for (int i = 1; i < size; i++)
    {
        MPI_Send(&slice_size, 1, MPI_INT, i, SLICE_SIZE, MPI_COMM_WORLD);
    }
}

/**
 * Genera el arreglo sobre el cual se va a hacer la busqueda y 
 * obtiene el valor que se va a buscar de manera aleatoria 
  */
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

/**
 * Llena el arreglo arr_wanted en base al indice que le corresponda a cada nodo
 */
void fill_arr_wanted()
{
    for (int j = 0; j < slice_size; j++)
    {
        arr_wanted[j] = array[indx + j];
    }
}

/**
 * Configura los valores del nodo maestro para que pueda ser tratado como un nodo esclavo y 
 * tambien realize una busqueda individual con los elementos que  
  */
void re_initiliaze_master()
{
    indx = 0;
    slice_size = module;
    fill_arr_wanted();
}

/**
 * Envia a los nodos esclavos el elemento que tiene que buscar, el arreglo donde tiene que buscar y 
 * el indice del arreglo original desde donde esta buscando 
  */
void send_data_to_search()
{
    for (int i = 1; i < size; i++)
    {
        fill_arr_wanted();
        MPI_Send(&indx, 1, MPI_INT, i, INDEX, MPI_COMM_WORLD);
        MPI_Send(arr_wanted, slice_size, MPI_INT, i, SLICE, MPI_COMM_WORLD);
        MPI_Send(&wanted, 1, MPI_INT, i, WANTED, MPI_COMM_WORLD);
        indx += slice_size;
    }

    //Busqueda individual en con residuos en master
    global_slice_size = slice_size;
    re_initiliaze_master();
}

/**
 * Recive los datos de busqueda enviados en el metodo send_data_to_search()
  */
void recive_data_to_search()
{
    MPI_Recv(&indx, 1, MPI_INT, 0, INDEX, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    MPI_Recv(arr_wanted, slice_size, MPI_INT, 0, SLICE, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    MPI_Recv(&wanted, 1, MPI_INT, 0, WANTED, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
}

/**
 * Cada nodo busca de manera individual la en la seccion del arreglo que le toca buscar
 * si encuntra un instancia del elemto buscado, guarda el indice en en arreglo arr_index
  */
void individual_search()
{
    int indx_count = 0;
    for (int i = 0; i < slice_size; i++)
    {
        if (arr_wanted[i] == wanted)
        {
            arr_index[indx_count] = indx;
            indx_count += 1;
        }
        indx += 1;
    }
}

/**
 * Metodo auxiliar para imprimir los resultados de busqueda que se le pasan por parametro
  */
void print_result(int arr_result[size][slice_size])
{
    printf("\n Resultado => ");
    for (int f = 0; f < size; f++)
    {
        for (int c = 0; c < slice_size; c++)
        {
            if (arr_result[f][c] != -1)
            {
                printf(" %d", arr_result[f][c]);
            }
        }
    }
    printf("\n");
}

/**
 * Metodo que recive los resultados de busqueda de cada nodo, y los guarda en un arreglo bidimencional
 * donde cada renglon son los resultados de busqueda del nodo cuyo identificador coincide con el indice del renglon
  */
void recive_report_results()
{
    slice_size = global_slice_size;
    int arr_result[size][slice_size];

    //Recive resultados
    for (int i = 1; i < size; i++)
    {
        MPI_Recv(arr_result[i], slice_size, MPI_INT, i, SEARCH_RESULT, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }

    //recopila los propios
    for (int j = 0; j < slice_size; j++)
    {
        arr_result[0][j] = arr_index[j];
    }

    //Imprime los resultados
    print_result(arr_result);
}

/**
 * Metodo que recopila los resultados de busqueda:
 * Los nodos esclavos envian sus resultados al nodo maestro
 * El nodo maestro recive los resultados, los recopila y los imprime en pantalla
  */
void collect_results()
{
    if (rank != 0)
    {
        MPI_Send(arr_index, slice_size, MPI_INT, 0, SEARCH_RESULT, MPI_COMM_WORLD);
    }
    else
    {
        recive_report_results();
    }
}

/**
 * Metodo auxiliar que imprime el aarreglo generado aleatoriamente, el elemento a buscar, y 
 * la cantidad de elementos que le toca procesar a cada nodo esclavo   
  */
void print_data_generated()
{
    printf("\nArray ");
    for (int i = 0; i < leng; i++)
    {
        printf(" %d", array[i]);
    }
    printf("\nwanted: %d, slice_size: %d\n", wanted, slice_size);
}

/**
 * Metodo que realiza una búsqueda distribuida sobre un arreglon generado de manera aleatoria
  */
int distributed_search(int argc, char **argv)
{
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (rank == 0)
    {
        //scan_wanted(); TODO arreglar o eliminar entrada
        initiliaze_master(argv);
        data_search();
        print_data_generated();
        send_data_to_search();
    }
    else
    {
        initiliaze_slave();
        recive_data_to_search();
    }

    individual_search();
    collect_results();

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
    //given
    if (rank == 0)
    {
        initiliaze_master(argv);
    }
    else
    {
        initiliaze_slave();
    }

    if (rank == 0)
    {
        data_search();
        send_data_to_search();
    }
    else
    {
        recive_data_to_search();
    }
    //then
    printf("\nnodo: %d\n", rank);
    printf("\nindx: %d\n", indx);
    printf("\nwanted: %d\n", wanted);
    printf("\nslice_size: %d\n", slice_size);
    printf("\n=====>Arr_wanted ");
    for (int i = 0; i < slice_size; i++)
    {
        printf(" %d", arr_wanted[i]);
    }
    printf("\n");
}

void test_individual_search(char **argv)
{
    //given
    if (rank == 0)
    {
        initiliaze_master(argv);
        data_search();
        send_data_to_search();
    }
    else
    {
        initiliaze_slave();
        recive_data_to_search();
    }
    individual_search();

    //then
    printf("\nnodo: %d\n", rank);
    printf("\nwanted: %d\n", wanted);
    printf("\nslice_size: %d\n", slice_size);
    printf("\n=====>Arr_wanted ");
    for (int i = 0; i < slice_size; i++)
    {
        printf(" %d", arr_wanted[i]);
    }
    printf("\n");

    printf("\n=====>Arr_index ");
    for (int i = 0; i < slice_size; i++)
    {
        printf(" %d", arr_index[i]);
    }
    printf("\n");
}

int test(int argc, char **argv)
{
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    //inicia prueba
    test_initiliaze_master(argv);
    //end de prueba

    MPI_Finalize();
    return 0;
}

/**
 * Funcion principal 
*/
int main(int argc, char **argv)
{
    inicializa();
    distributed_search(argc, argv);
    //test(argc, argv);
}