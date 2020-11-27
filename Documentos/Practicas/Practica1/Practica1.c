#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define TAG_VUELTAUNO 4
#define TAG_VUELTADOS 5
#define ATTACT 1
#define REGRET 2


//Estructuras
int GENERALS; // cuenta desde 0 i.e son 3
int* plan;            // 2 -> No ataca, 1 -> ataca
int* majorityPlan;
int* plan_bizantino;
int election;
char bizantino;
int s;

void f_bizantino()
{
    for (int i = 0; i <= GENERALS; i++)
    {
        if (plan[i] == ATTACT)
        {
            plan_bizantino[i] = REGRET;
        }else if (plan[i] == REGRET)
        {
            plan_bizantino[i] = ATTACT;
        }
    }
    
}

/** 
 * Choose
*/
int choose(int attact, int regret)
{
    if (regret >= attact)
    {
        return REGRET;
    }
    return ATTACT;
}

/**
 *  Majority
*/
int majority(int plan, int reportedPlan[][s], int general)
{
    int attact = 0;
    int reject = 0;
    for (int i = 0; i <= GENERALS; i++)
    {
        //Cuenta reportados
        if (reportedPlan[general][i] == ATTACT)
        {
            attact++;
        }
        else if (reportedPlan[general][i] == REGRET)
        {
            reject++;
        }
    }
        //Cuenta el propio
        if (plan == ATTACT)
        {
            attact++;
        }
        else if(plan == REGRET)
        {
            reject++;
        }
        
    return choose(attact, reject);
}

/**
 *  Final Majority
*/
int majorityFinal()
{
    int attact = 0;
    int regret = 0;
    for (int i = 0; i <= GENERALS; i++)
    {
        //Cuenta reportados
        if (majorityPlan[i] == ATTACT)
        {
            attact++;
        }
        else if(majorityPlan[i] == REGRET)
        {
            regret++;
        }

    }
    return choose(attact, regret);
}


/**
 * Primera vuelta 
*/
void firstRound(int rank){
    //Envia
    for (int g = 0; g <= GENERALS; g++)
    {
        //A quien se envia nuestro pla
        if (g != rank)
        {
            MPI_Send(&plan[rank], 1, MPI_INT, g, TAG_VUELTAUNO, MPI_COMM_WORLD);
        }
    }

    //recive
    for (int g = 0; g <= GENERALS; g++)
    {
        if (g != rank)
        {
            MPI_Recv(&plan[g], 1, MPI_INT, g, TAG_VUELTAUNO, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
    }
}

/** 
 * Segunda vuelta 
*/
void secondRound(int rank,int planEnviado[], int reportedPlan[][s]){
    //Envia
    for (int g = 0; g <= GENERALS; g++)
    {
        //De quien es el plan que se envia; de g
        if (g != rank) // No se quiren enviar los planes de ejecucion del nodo que se esta ejecutando
        {
            //A quien enviamos el plan; a gp
            for (int gp = 0; gp <= GENERALS; gp++)
            {
                if (gp != g) // Si es el nodo del cual se esta enviando su plan, a ese nodo no se le reenvia su plan
                {
                    MPI_Send(&planEnviado[g], 1, MPI_INT, gp, TAG_VUELTADOS, MPI_COMM_WORLD); //se envia a gp el plan de g, NO se le renvia su plan a G
                }
                
            }
        }
    }
    //Recibe
    for (int g = 0; g <= GENERALS; g++)
    {
        if (g != rank) //No esperamos recibir mensaje del nodo que se esta ejecutando
        {
            for (int gp = 0; gp <= GENERALS; gp++)
            {
                if (gp != g)//No se quiere recibir el plan de g
                {
                    MPI_Recv(&reportedPlan[g][gp], 1, MPI_INT, gp, TAG_VUELTADOS, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                }
                
            }
        }
    }
}

/**
 * First vote 
*/
void firstVote(int reportedPlan[][3]){
    for (int g = 0; g <= GENERALS; g++)
    {
        majorityPlan[g] = majority(plan[g], reportedPlan ,g);
    }
}

/**
 * Second vote
*/
void secondVote(int rank){
    majorityPlan[rank] = plan[rank];
    int accion = majorityFinal(majorityPlan);
    printf("\nNodo: %d, accion: %d\n", rank, accion);
}

/**
 * Algoritmo generales 
*/
int generales(int argc, char **argv){
    int size, rank;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int reportedPlan[s][s];
    election = *argv[(2*(rank+1))] - '0';
    bizantino = *argv[2*(rank+1)+1];
    plan[rank] = election;


    firstRound(rank);

    if (bizantino == 'B')
    {
        f_bizantino();
        secondRound(rank, plan_bizantino, reportedPlan);
    }else if (bizantino == 'L')
    {
        secondRound(rank, plan, reportedPlan);
    }
    
    firstVote(reportedPlan);
    secondVote(rank);

    MPI_Finalize();
    return 0;

}

/**
 * Para ejecutar el programa completo descomentar el metodo generales y comentar test, 
 * Para hacer pruebas descomentar generales y comentar test
*/

int main(int argc, char **argv)
{
    GENERALS = (*argv[1] - '0') -1;
    s = GENERALS+1;
    plan = (int*) malloc((GENERALS+1)* sizeof(int));
    majorityPlan = (int*) malloc((GENERALS+1)* sizeof(int));
    plan_bizantino = (int*) malloc((GENERALS+1)* sizeof(int));  


    //test(argc,argv);
    generales(argc, argv);

    return 0;
}
