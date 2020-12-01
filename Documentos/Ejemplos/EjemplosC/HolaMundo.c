#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void inicializa(){
	time_t t;
	srand(time(&t));
}

int main()
{
    inicializa();
    int j = rand() % 10;
    printf("valor de j = %d\n", j);
}