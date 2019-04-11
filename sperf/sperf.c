//Makefile 主要参考了libco/tests中的Makefile;make run 是 64 与 32 bit的test,另外还有make run32 ; make run64;
#include <stdio.h>

#include <unistd.h>
#define Log(format, ...) \
    printf("\33[1;35m[%s,%d,%s] " format "\33[0m\n", \"]]")
void test();
int main(int argc, char *argv[],char *envp[]) {
  
  printf("Hello world!!\n");

  for(int i=0;i<argc;i++)
  {
    Log("argv : %s\n",argv[i]);
  }
//  test(argv,envp);
  //strace -T ls
  return 0;
}
void test(char *argv[],char *envp[]){
char *argva[]={"strace","-T",argv[1],NULL};//传递给执行文件的参数数组，这里包含执行文件的参数 

  execve("/usr/bin/strace",argva,envp);

}
