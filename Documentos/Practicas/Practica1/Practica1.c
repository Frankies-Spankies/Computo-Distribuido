#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define TAG_VUELTAUNO 4
#define TAG_VUELTADOS 5
#define ATTACT 1
#define REGRET 2

//Estructuras
const int GENERALS = 2; // cuenta desde 0 i.e son 3
int plan[3];            // 2 -> No ataca, 1 -> ataca
int reportedPlan[3][3];
int majorityPlan[3];

/** 
 * Choose
*/
int choose(int attact, int reject)
{
    if (reject >= attact)
    {
        return REGRET;
    }
    return ATTACT;
}

/**
 *  Majority
*/
int majority(int plan, int reportedPlan[][3], int general, int generals)
{
    int attact = 0;
    int reject = 0;
    for (int i = 0; i < generals; i++)
    {
        //Cuenta reportados
        if (reportedPlan[i][general] == ATTACT)
        {
            attact++;
        }
        else if (reportedPlan[i][general] == REGRET)
        {
            reject++;
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
}

/**
 *  Final Majority
*/
int majorityFinal(int majorityPlan[], int generals)
{
    int attact = 0;
    int reject = 0;
    for (int i = 0; i < generals; i++)
    {
        //Cuenta reportados
        if (majorityPlan[i] == ATTACT)
        {
            attact++;
        }
        else if(majorityPlan[i] == REGRET)
        {
            reject++;
        }

        return choose(attact, reject);
    }
}


/**
 * Primera vuela 
*/
void firstRound(int rank){
    //Envia
    for (int g = 0; g <= GENERALS; g++)
    {
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
void secondRound(int rank){
    //Envia
    for (int g = 0; g <= GENERALS; g++)
    {
        if (g != rank)
        {
            for (int gp = 0; gp <= GENERALS; gp++)
            {
                MPI_Send(&plan[g], 1, MPI_INT, gp, TAG_VUELTAUNO, MPI_COMM_WORLD);
            }
        }
    }
    //Recibe
    for (int g = 0; g <= GENERALS; g++)
    {
        if (g != rank)
        {
            for (int gp = 0; gp <= GENERALS; gp++)
            {
                MPI_Recv(&reportedPlan[g][gp], 1, MPI_INT, gp, TAG_VUELTAUNO, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            }
        }
    }
}

/**
 * First vote 
*/
void firstVote(){
    for (int g = 0; g <= GENERALS; g++)
    {
        majorityPlan[g] = majority(plan[g], reportedPlan, g, GENERALS);
    }
}

/**
 *Second vote
*/
void secondVote(int rank){
    majorityPlan[rank] = plan[rank];
    int accion = majorityFinal(majorityPlan, GENERALS);
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

    int input = *argv[rank+1] - '0';
    plan[rank] = input;


    firstRound(rank);
    secondRound(rank);
    firstVote();
    secondVote(rank);

    MPI_Finalize();
    return 0;

}




void test_firstRound(int rank){
    firstRound(rank);
    printf("\nNodo: %d, accion:%d\n", rank, plan[rank] );
    
}

void test_secondRound(int rank){
    plan[0]=1;
    plan[1]=1;
    plan[2]=1;
    secondRound(rank);

    printf("\nNodo: %d ==\n", rank);
    for (int f = 0; f < 3; f++)
    {
        for (int c = 0; c < 3; c++){
            printf("%d", reportedPlan[f][c]);
        }
        printf("\n");
    }
}

void test_firstVote(int rank){
    firstRound(rank);
    secondRound(rank);
    firstVote();

    char majorityPlanCadena[3];
    
    for (int i = 0; i < 3; i++)
    {
        majorityPlanCadena[i] = majorityPlan[i] + '0';
    }
    printf("\nNodo: %d, majorityPlan: [%s]\n", rank, majorityPlanCadena);
    
}

/**
 *  Test 
*/
int test(int argc, char **argv){
    int size, rank;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int input = *argv[rank+1] - '0';
    plan[rank] = input;    


    //Codigo a probar

    test_firstVote(rank);

    //Fin del test

    MPI_Finalize();
    return 0;    
}


int main(int argc, char **argv)
{
    //test(argc,argv);
    generales(argc, argv);

    return 0;
}
