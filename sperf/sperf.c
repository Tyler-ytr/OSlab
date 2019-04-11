//Makefile 主要参考了libco/tests中的Makefile;make run 是 64 与 32 bit的test,另外还有make run32 ; make run64;
#include <stdio.h>

#include <unistd.h>
void test();
int main(int argc, char *argv[],char *envp[]) {
  
  printf("Hello world!!\n");

  for(int i=1;i<argc;i++)
  {
    printf("argv : %s\n",argv[i]);
  }
char *argva[]={"strace","-T",argv[1],NULL};//传递给执行文件的参数数组，这里包含执行文件的参数 

  execve("/usr/bin/strace",argva,envp);
//  test(argv);
  //strace -T ls
  return 0;
}
void test(char *argv[]){

    printf("argv : %s\n",argv[1]);
char *argva[]={"/usr/bin/strace","-T",argv[1],">/dev/null",NULL};//传递给执行文件的参数数组，这里包含执行文件的参数 

  char *envp[]={0,NULL};//传递给执行文件新的环境变量数组
  execve("/usr/bin/strace",argva,envp);
  printf("here\n");
}
