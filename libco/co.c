#include <stdio.h>
#include <ucontext.h>
#include <unistd.h>
#include <assert.h>
#include "co.h"
//借助了ucontext库,主要使用int getcontext(ucontext_t *ucp);int setcontext(const ucontext_t *ucp);void makecontext(ucontext_t *ucp, void (*func)(), int argc, ...);int swapcontext(ucontext_t *oucp, ucontext_t *ucp);四个库中的函数来实现协程
//
//使用协程的程序会首先调用co_init完成一些必要的初始化。如果你的实现并不需要在启动时做任何初始化，你可以留下一个空的函数。
/*extern int getcontext(ucontext_t *ucp);
extern int setcontext(const ucontext_t *ucp);
extern void makecontext(ucontext_t *ucp, void (*func)(), int argc, ...);
extern int swapcontext(ucontext_t *oucp,const  ucontext_t *ucp);
*/
struct co {
    ucontext_t ctx;
    int id;//在coroutins里面的位置;
    const char *name;
    func_t func;
    void* arg;
    enum STATUS status;//DEAD/READY/RUNNING/SUSPEND
    char stack[STACK_SIZE];
};
int _TOTAL=0;//总的进程数
int _NOW=-1;//当前在跑的进程,如果没有为-1;
struct co coroutines[MAX_CO];
//co_start创建一个新的协程，并返回一个指针(动态分配内存)。我们的框架代码中并没有限定 struct co 结构体的设计，所以你可以自由发挥

void co_init() {
}

void thread_body()
{

  //To be continued of the if(id==?)
  int id=_NOW;
  if(id!=-1)
  {
    struct co* current=&coroutines[id];
    current->func(current->arg);
    _NOW=-1;
  }
  else
  {
      printf("Awsl:  GG in thread_body\n");
      assert(0);
  }
  return;
  

}


struct co* co_start(const char *name, func_t func, void *arg) {
  func(arg); // Test #2 hangs
    
  //strucinest co current=coroutines[_TOTAL];
  coroutines[_TOTAL].id =_TOTAL;
  coroutines[_TOTAL].name=name;
  coroutines[_TOTAL].func=func;
  coroutines[_TOTAL].arg=arg;
  coroutines[_TOTAL].status=READY;

  getcontext(&(coroutines[_TOTAL].ctx));
  coroutines[_TOTAL].ctx.uc_stack.ss_sp = coroutines[_TOTAL].stack;
  coroutines[_TOTAL].ctx.uc_stack.ss_size = STACK_SIZE;

  coroutines[_TOTAL].ctx.uc_stack.ss_flags=0;
  _NOW=_TOTAL;
  printf("here\n");
  makecontext(&(coroutines[_TOTAL].ctx),(void (*)(void))thread_body,1);
  printf("hhere\n");


  //assert(0);
  struct co* current=&coroutines[_TOTAL];

  _TOTAL++;
//  return NULL;
  return current;
}

//co_yield 是指当前运行的协程放弃执行，并切换到其他协程执行。系统中可能有多个运行的协程(包括当前协程)。你可以随机选择下一个系统中可运行的协程
void co_yield() {

 /* ucontext_t now;
  int check=getcontext(&now);//now获取当前上下文
  assert(check!=-1);//为-1的时候就凉了( ⊙ o ⊙  )！
*/

  if(_NOW!=-1)
  {
    
      printf("here");

  }else
  {
    printf("Awsl:There are no coroutine running now;\n");
    //assert(0);
  }





}

//co_wait(thd) 表示当前协程不再执行，直到 thd 协程的执行完成。我们规定，每个协程的资源在co_wait()等待结束后释放，因此每个协程只能被co_wait一次。更精确地说，每个协程必须恰好被co_wait一次，否则就会造成资源泄露。
void end_and_free()
{
    ;
}
void co_wait(struct co *thd) {
}


