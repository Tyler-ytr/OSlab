#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
void Exitcrepl();
void Somethingwrong(char *str)
{
    perror(str);
    Exitcrepl();
    exit(EXIT_FAILURE);
}

void Exitcrepl()
{
    if(system("rm -rf test")!=0)
    {
        perror("rm -rf test");
        assert(0);
    }
    if(system("rm -rf lib")!=0)
    {
        perror("rm -rf lib");
        assert(0);
    }

}

int main(int argc, char *argv[]) {
    if(system("mkdir test")!=0)
        Somethingwrong("mkdir test");
    if(system("mkdir lib")!=0)
        Somethingwrong("mkdir lib");


    Exitcrepl();
    printf("Success");


    return 0;
}
