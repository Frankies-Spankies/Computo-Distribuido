#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define TAG_VUELTAUNO 1
#define TAG_VUELTADOS 2

//Estructuras
const int GENERALS = 2; // cuenta desde 0 i.e son 3
int plan[3];            // 0 -> No ataca, 1 -> ataca
int reportedPlan[3][3];
int majorityPlan[3];

/** 
 * Choose
*/
int choose(int attact, int reject)
{
    if (reject >= attact)
    {
        return 0;
    }
    return 1;
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
        if (reportedPlan[i][general] == 1)
        {
            attact++;
        }
        else if (reportedPlan[i][general] == 0)
        {
            reject++;
        }

        //Cuenta el propio
        if (plan == 1)
        {
            attact++;
        }
        else if(plan == 0)
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
        if (majorityPlan[i] == 1)
        {
            attact++;
        }
        else
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
int secondVote(int rank){
    majorityPlan[rank] = plan[rank];
    return majorityFinal(majorityPlan, GENERALS);
}



void test_firstRound(int rank){
    firstRound(rank);
    printf("\nNodo: %d, accion:%d\n", rank, plan[rank] );
    
}

void test_secondRound(int rank){
    plan[0]=1;
    plan[1]=1;
    plan[2]=0;
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

    printf("\nNodo: %d ==\n", rank);
    for (int i = 0; i < 3; i++)
    {
        printf("algo: %d", majorityPlan[i]);
    }
    
}

int main(int argc, char **argv)
{
    int size, rank;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    plan[0] = 1;
    plan[1] = 0;
    plan[2] = 1;

    //test_secondRound(rank);
    firstRound(rank);
    secondRound(rank);
    firstVote();
    int accion = secondVote(rank);


    printf("\nNodo: %d, accion: %d\n", rank, accion);

 
    MPI_Finalize();
    return 0;
}
