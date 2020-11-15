#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define TAG_VUELTAUNO 1
#define TAG_VUELTADOS 2

int choose(int attact, int reject)
{
    if (attact > reject)
    {
        return 1;
    }
    return 0;
}

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
        else if (reportedPlan[i][general] == 1)
        {
            reject++;
        }

        //Cuenta el propio
        if (plan == 1)
        {
            attact++;
        }
        if (reportedPlan[i][general] == 1)
        {
            reject++;
        }

        choose(attact, reject);
    }
}

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

        choose(attact, reject);
    }
}

int main(int argc, char **argv)
{
    // Inicio OpenMPI
    int size, rank;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    //Ejecutar

    //Etructuras
    const int GENERALS = 2; // cuenta desde 0 i.e son 3
    int eleccion = 1;  // 0 -> No ataca, 1 -> ataca
    int plan[3];
    int reportedPlan[3][3];
    int majorityPlan[3];

    /* Asignacion por nodo */
    plan[rank] = 1;

    /* Primera vuelta */
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

    /* Segunda vuelta */
    //Envia
    for (int g = 0; g <= GENERALS; g++)
    {
        if (g != rank)
        {
            //Envia arreglo planes
            for (int gp = 0; gp < GENERALS; gp++)
            {
                MPI_Send(&plan[g], 1, MPI_INT, gp, TAG_VUELTAUNO, MPI_COMM_WORLD);
            }
        }
    }

    //Recive
    for (int g = 0; g <= GENERALS; g++)
    {
        if (g != rank)
        {
            //Envia arreglo planes
            for (int gp = 0; gp < GENERALS; gp++)
            {
                MPI_Recv(&reportedPlan[g][gp], 1, MPI_INT, gp, TAG_VUELTAUNO, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            }
        }
    }

    /* Primer voto */
    for (int g = 0; g <= GENERALS; g++)
    {
        majorityPlan[g] = majority(plan[g], reportedPlan, g, GENERALS);
    }

    /* Segundo voto */
    majorityPlan[rank] = plan[rank];
    return majorityFinal(majorityPlan, GENERALS);

    //Fin de ejecucion
    MPI_Finalize();
    return 0;
}
