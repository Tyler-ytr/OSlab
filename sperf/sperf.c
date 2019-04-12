//Makefile 主要参考了libco/tests中的Makefile;make run 是 64 与 32 bit的test,另外还有make run32 ; make run64;
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>

#define maxn 4096
int flides[2];//flides[0]:read end of the pipe;flides[1]:write end of the pipe;
struct {
  char func_name[maxn][50];
  int num;
  double func_time[maxn];
  double total_time;
}G;//全局的记录关于程序的整个信息;

void init();
void test();
int main(int argc, char *argv[],char *envp[]) {
  if(argc<2)
  {
    printf("Don't know how to use? ./sperf-32/64 [arg1] [arg2]....\n");
  }
  for(int i=0;i<argc;i++)
  {
    printf("argc %d : %s \n",i,argv[i]);
  }
  

  if((pipe(flides))!=0)//根据rtfm,正常返回值为0;初始化flides;
  {
    printf("Error: Pipe is wrong!!\n");
    assert(0);
  }

  init();

  pid_t pid;
  pid=fork();
  if(pid<0)
  {
    printf("Error: fork is wrong!!\n");
    assert(0);
  }

  if(pid==0)
  {
    close(flides[0]);//子进程关闭读;
char *argva[]={"strace","-T",argv[1],NULL};//传递给执行文件的参数数组，这里包含执行文件的参数 
  
  execve("/usr/bin/strace",argva,envp);

  }
  else{
    //To be continued;
    close(flides[1]);//父进程关闭写
    init();
  }

//  test(argv,envp);
  //strace -T ls
  return 0;
}
void test(char *argv[],char *envp[]){
char *argva[]={"strace","-T",argv[1],NULL};//传递给执行文件的参数数组，这里包含执行文件的参数 

  execve("/usr/bin/strace",argva,envp);

}

void init(){
  G.num=0;
  memset(G.func_name,'\0',sizeof(G.func_name));
  memset(G.func_time,0.000,sizeof(G.func_time));
  G.total_time=0.000;
}


