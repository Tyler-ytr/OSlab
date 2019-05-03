#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void Somethingwrong(char *str)
{
    perror(str);

    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]) {
    if(system("mkdir test")!=0)
        Somethingwrong("mkdir test");
    printf("Success");


    return 0;
}
