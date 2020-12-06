#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

void inicializa()
{
    time_t t;
    srand(time(&t));
}

int main(int argc, char **argv)
{
    char buffer[100];
    int number;
fgets(buffer, sizeof buffer, stdin);
        // if(sscanf(buffer, "%d", &number) != 1) {    // or strtol perhaps
        // }
        sscanf(buffer, "%d", &number);
        printf("Number: %d\n", number+1);
    
    return 0; 

}
