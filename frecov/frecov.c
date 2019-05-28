#include <sys/mman.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <fcntl.h>
#include<stdlib.h>
#include <sys/stat.h> 

 typedef struct mbr{
   int8_t test;
 }MBR;





int main(int argc, char *argv[]) {
struct stat file_stat;
struct MBR *test1;
  for(int i=0;i<argc;i++)
  {
    printf("argc %d : %s \n",i,argv[i]);
  }

  void * start;
  int fd;
  fd=open(argv[1],O_RDONLY);
  if(fd==-1){
    perror("Can't find the file!");
    return 0;
  }
  if(fstat(fd,&file_stat)==-1){
    perror("Fstat error!");
    return 0;
  };
  
  start=mmap(0,file_stat.st_size ,PROT_READ,MAP_SHARED,fd,0);
  if(start==(void*)-1){
    perror("Mmap!");
    return 0;
  }
  printf("Success mmap!");
  close(fd);
  test1=start;

  
  printf("0x%p",(void *)test1);

  return 0;
}
