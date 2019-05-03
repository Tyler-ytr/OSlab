#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
void Somethingwrong(char *str)
{
    perror(str);
    Exitcrepl();
    exit(EXIT_FAILURE);
}

void Exitcrepl()
{
    if(system("rm -rf test")!=0)
        perror("rm -rf test");
        assert(0);

}

int main(int argc, char *argv[]) {
    if(system("mkdir test")!=0)
        Somethingwrong("mkdir test");
    printf("Success");


    return 0;
}
