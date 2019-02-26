#include <stdio.h>
#include <assert.h>
#include <dirent.h>

//I use stat to get the information the the documents;
//I use scandir to read the catalog

void test()
{
    struct dirent **namelist;
    int n;
    n=scandir(".",&namelist,0,alphasort);
    if(n<0)
    {
        perror("scandir");
    }
    else
    {
        while(n--)
        {
            printf("%s/n",namelist[n]->d_name);
            free(namelist[n]);
        }
        free(namelist);
    }
}



int main(int argc, char *argv[]) {
  printf("Hello, World!\n");
  int i;
  for (i = 0; i < argc; i++) {
    assert(argv[i]); // always true
    printf("argv[%d] = %s\n", i, argv[i]);
  }
  printf("I am here\n");
  assert(!argv[argc]); // always true
  printf("I am here\n");
  test();
  return 0;
}







