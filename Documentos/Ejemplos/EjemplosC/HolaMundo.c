#include <stdio.h>

void jugar(int* n){
    int y = *n ;
    y = 2 * y;
    *n = y;
    printf("valor de n = %d", *n);
}

int main()
{
    int j = 5;
    jugar(&j);
    printf("valor de j = %d", j);
}