#include <common.h>
#include "x86.h"

static void kmt_spin_init(spinlock_t *lk, const char *name);
static void kmt_spin_lock(spinlock_t *lk);
static void kmt_spin_unlock(spinlock_t *lk);


static int ncli[9]={0,0,0,0,0,0,0,0,0};
static int intena[9]={0,0,0,0,0,0,0,0,0};

static spinlock_t sem_lock;//信号量里面使用;
static spinlock_t task_lock;//在kmt_create,kmt_teardown里面使用,操作task链表;
//static task_t * current_task=NULL;
static task_t * task_head[9];//task 链表的头部; 每一个cpu对应一个头部;
static task_t * current_task[9]//当前的进程;
//static int task_length=0;
static const int _non=0,_runningable=1,_running=2,_waiting=3;
//0 没有初始化 1 runningable 2 running 3 waiting 

//static inline void panic(const char *s) { printf("%s\n", s); _halt(1); }
static void kmt_init(){
  //current_task=NULL;
  for(int i=0;i<9;i++)
  {
    task_head[i]=NULL;
    current_task=NULL;
  }
  //int task_length=0;
  kmt_spin_init(&sem_lock,"sem_lock");
  kmt_spin_init(&task_lock,"task_lock");
 
  os->on_irq(INT8_MIN, _EVENT_NULL, kmt_context_save); // 总是最先调用
  os->on_irq(INT8_MAX, _EVENT_NULL, kmt_context_switch); // 总是最后调用
    //TO BE DONE
    return;
}

static int kmt_create(task_t *task, const char *name, void (*entry)(void *arg), void *arg){
    TRACE_ENTRY;
    //TO BE DONE
    kmt_spin_lock(&task_lock);
    //------------原子操作------------------ 
    //Log1("Before allocate in create");
    task->stack.start=pmm->alloc(MAX_STACK_SIZE);
    //Log1("finish task start alloc");
    task->stack.end=task->stack.start + MAX_STACK_SIZE;
    task->status=_runningable;
    task->name=name;
    task->context=*_kcontext(task->stack, entry, arg);//上下文上吧; 在am.h以及cte.c里面有定义;

    task_t * new_task=task;

    //c--------head-->a-->b-->NULL-->>>>head-->c-->a-->b-->NULL
    if(task_head[(int)_cpu()]==NULL)
    {
      new_task->next=NULL;
      task_head[(int)_cpu()]=new_task;
    }
    else
    {
      new_task->next=task_head[(int)_cpu()];//此时head是a
      task_head[(int)_cpu()]=new_task;//把头变成c;
    }
    
    //-------------原子操作-----------------
    kmt_spin_unlock(&task_lock);
    TRACE_EXIT;
    return 0;
}

static void kmt_teardown(task_t *task){
    //  从head 数组开始遍历,找到它然后释放;
    //TO BE DONE
    TRACE_ENTRY; 
    kmt_spin_lock(&task_lock);
    //------------原子操作------------------ 
    pmm->free(task->stack.start);
    int success_find=0;
    task_t *now=NULL;
    for(int i=0;i<9;i++)
    { now=task_head[i];
      if(task_head[i]==NULL)continue;
      else if(task_head[i]==task){
        task_head[i]=now->next;
        success_find=1;
      }
      else{
       while(now->next!=NULL){
          if(task==now->next){
            task_t *find=now->next;
            now->next=find->next;
            success_find=1;
            break; 
          }
          now=now->next;
       } 
      }
    }

    if(success_find==0)
    {
      panic("In kmt_teardown, can't find the task!!");
    }
    //-------------原子操作-----------------
    kmt_spin_unlock(&task_lock);
    TRACE_EXIT;
    return;
}

static void kmt_spin_init(spinlock_t *lk, const char *name){

    lk->name=name;
    lk->locked=0;
    lk->cpu=-1;
    //TO BE DONE
    return ;
}



static inline uint
xchg(volatile uint *addr, uint newval)
{
  uint result;

  // The + in "+m" denotes a read-modify-write operand.
  asm volatile("lock; xchgl %0, %1" :
               "+m" (*addr), "=a" (result) :
               "1" (newval) :
               "cc");
  return result;
}
static void
popcli(void)
{
  if(get_efl()&FL_IF)//readflags==get_efl
    panic("popcli - interruptible");
  if(--ncli[(int)_cpu()] < 0)
   { printf("cpu: %d %d: ",(int)_cpu(),ncli[(int)_cpu()]);
    panic("popcli");}
 // if(--ncli[(int)_cpu()] == 0 && mycpu()->intena)
 if(ncli[(int)_cpu()] == 0&&intena[(int)_cpu()])
    sti();
 /*ncli[(int)_cpu()]--;
    //printf("pop ncli: cpu %d ncli[_cpu]:%d \n",_cpu(),ncli[(int)_cpu()]);
  assert(ncli[(int)_cpu()]>=0);
  if(ncli[(int)_cpu()]==0)
  {

    sti();
  }
*/
}
static void
pushcli(void)
{
  int eflags;

  eflags = get_efl();
  cli();
  if(ncli[(int)_cpu()] == 0)
   intena[(int)_cpu()] = eflags & FL_IF;
 ncli[(int)_cpu()]  += 1;
}
static int
holding(spinlock_t *lock)
{
  int r;
  pushcli();
  r = lock->locked && lock->cpu == (int)_cpu();
 popcli();
  return r;
}


static void kmt_spin_lock(spinlock_t *lk){
 pushcli(); // disable interrupts to avoid deadlock.
  if(holding(lk))
    {
     printf("%d %d %d, ", lk->locked,lk->cpu,(int)_cpu()); 
      panic("Spin_lock");}

  // The xchg is atomic.
  while(xchg(&lk->locked, 1) != 0)
    ;

  // Tell the C compiler and the processor to not move loads or stores
  // past this point, to ensure that the critical section's memory
  // references happen after the lock is acquired.
  __sync_synchronize();

  // Record info about lock acquisition for debugging.
  lk->cpu = (int)_cpu();
  //getcallerpcs(&lk, lk->pcs);
}


static void kmt_spin_unlock(spinlock_t *lk){


 if(!holding(lk))
    panic("Unlock");

//  lk->pcs[0] = 0;
  lk->cpu = -1;

  // Tell the C compiler and the processor to not move loads or stores
  // past this point, to ensure that all the stores in the critical
  // section are visible to other cores before the lock is released.
  // Both the C compiler and the hardware may re-order loads and
  // stores; __sync_synchronize() tells them both not to.
  __sync_synchronize();

  // Release the lock, equivalent to lk->locked = 0.
  // This code can't use a C assignment, since it might
  // not be atomic. A real OS would use C atomics here.
  asm volatile("movl $0, %0" : "+m" (lk->locked) : );

  popcli();

    //TO BE DONE
    return ;
}
static void kmt_sem_init(sem_t *sem, const char *name, int value){
  sem->value=value;
  sem->MAXSIZE=256;
  sem->name=name;
//  kmt_spin_init(sem->lock);  
  sem->end=0;
  sem->start=sem->MAXSIZE;//当head%MAXSIZE与tail%MAXSIZE相等的时候,队列是空的;
                          //当(tail+1)%MAXSIZE与head%MAXSIZE相等的时候,队列是满的;
  return ;
}

static void kmt_sem_wait(sem_t *sem){
  kmt_spin_lock(&sem_lock);
  //------------原子操作------------------ 
  sem->value--;
  if(current_task[(int)_cpu()]==NULL){
    panic("In sem_wait No task in this cpu");
  }
  //---------对于每一个sem: tasklist:
  //----先进先出；参考数据结构:https://blog.csdn.net/a04081122/article/details/51985873
  while(sem->value<0){
    current_task[(int)_cpu()]->status=_waiting;
    //sem->end++;
    if(((sem->end+1)%sem->MAXSIZE)==(sem->start%sem->MAXSIEZ))panic("In sem_wait, the task_list is full;");
    //int if_sleep;
    sem->task_list[sem->end]=current_task[_cpu()];
    sem->end++;
    sem->end%=sem->MAXSIZE;
    kmt_spin_unlock(&sem_lock);
    _yield();
    kmt_spin_lock(&sem_lock);
    //理论上不可能发生进入两次队列的情况 如果后面有bug可以在这个地方加一个assert;
  }
  //------------原子操作------------------ 
  kmt_spin_unlock(&sem_lock);

  return;
}

static void kmt_sem_signal(sem_t *sem){
  kmt_spin_lock(&sem_lock);
  //------------原子操作------------------ 
  sem->value++;
  

  //------------原子操作------------------ 
  kmt_spin_unlock(&sem_lock);
    return;
}



  





MODULE_DEF(kmt) {
  .init   = kmt_init,
  .create    = kmt_create,
  .teardown   = kmt_teardown,
  .spin_init=kmt_spin_init,
  .spin_lock=kmt_spin_lock,
  .spin_unlock=kmt_spin_unlock,
  .sem_init=kmt_sem_init,
  .sem_wait=kmt_sem_wait,
  .sem_signal=kmt_sem_signal,
};
