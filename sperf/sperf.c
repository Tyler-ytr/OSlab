//Makefile 主要参考了libco/tests中的Makefile;make run 是 64 与 32 bit的test,另外还有make run32 ; make run64;
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <fcntl.h>

#define maxn 4096
int fd[2];//fd[0]:read end of the pipe;fd[1]:write end of the pipe;
struct {
  char func_name[maxn][50];
  int num;
  double func_time[maxn];
  double total_time;
}G;//全局的记录关于程序的整个信息;
char buffer[maxn];//用于fgets时候的缓冲区;
void init();
void test();
void calculate(char origin[]);
int main(int argc, char *argv[],char *envp[]) {
  if(argc<2)
  {
    printf("Don't know how to use? ./sperf-32/64 [arg1] [arg2]....\n");
  }
  for(int i=0;i<argc;i++)
  {
    printf("argc %d : %s \n",i,argv[i]);
  }
  

  if((pipe(fd))!=0)//根据rtfm,正常返回值为0;初始化fd;
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
    close(fd[0]);//子进程管道关闭读;
char *argva[]={"strace","-T",argv[1],NULL};//传递给执行文件的参数数组，这里包含执行文件的参数 

  int fd_null=open("/dev/null",O_WRONLY);//参考open手册,只写地搞到null的文件描述符;
  if(fd_null<0)
  {
    printf("Error:Fail to open /dev/null\n");
    assert(0);
  }
   dup2(fd[1], STDERR_FILENO);//用fd[1](管道写入端)代替stderr;
   dup2(fd_null,STDOUT_FILENO);//关闭stdout的输出,输出到null里面去;
  execve("/usr/bin/strace",argva,envp);
  
  assert(0);

  }
  else{
    //To be continued;
    close(fd[1]);//父进程关闭写
    init();
    dup2(fd[0],STDIN_FILENO);//用管道里面的读入端内容代替stdin;
    while(fgets(buffer,maxn,stdin)!=NULL)
    {
      //printf("%s",buffer);
      //printf("\n\n\n\n\n");
      calculate(buffer);
    }






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

void calculate(char origin[]){
  //正则表达式;需要将开头的括号前的内容(也就是一些英文捕获),需要将<>中的浮点数捕获;
  char name[50];
  double time;
  sscanf(origin,"%[0-9|a-z|A-Z]",name);
  sscanf(origin,"<%lf>",&time);

  printf("%s:",name);
  printf("%lf\n\n",time);



  ;
}

//参考网站:
//正则：https://www.cnblogs.com/youthlion/archive/2009/06/21/1507586.html
//dev/null:https://www.zhihu.com/question/53295083/answer/135258024
//execve:https://blog.csdn.net/fisher_jiang/article/details/5608399
//fork,pipe:https://blog.csdn.net/wumenglu1018/article/details/54019755
//dup:https://blog.csdn.net/zhouhong1026/article/details/8151235
//dup:https://www.cnblogs.com/GODYCA/archive/2013/01/05/2846197.html
