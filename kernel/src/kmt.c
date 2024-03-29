#include <common.h>
#include "x86.h"

static void kmt_spin_init(spinlock_t *lk, const char *name);
static void kmt_spin_lock(spinlock_t *lk);
static void kmt_spin_unlock(spinlock_t *lk);
static _Context *kmt_context_save(_Event ev, _Context *context);
static _Context *kmt_context_switch(_Event ev, _Context *context);
static int kmt_create_init(task_t *task, const char *name, void (*entry)(void *arg), void *arg,int cpu);
static void cpu_task(void *arg);
extern void * procfs_add(const char *name);
//static void cpu1_task(void *arg);
//static void cpu2_task(void *arg);
//static void cpu3_task(void *arg);



static int ncli[9]={0,0,0,0,0,0,0,0,0};
static int intena[9]={0,0,0,0,0,0,0,0,0};

static spinlock_t sem_lock;//信号量里面使用;
static spinlock_t task_lock;//在kmt_create,kmt_teardown里面使用,操作task链表;
static spinlock_t context_lock;//在switch 以及 save里面使用;
//static spinlock_t yield_lock;//在切换
//static task_t * current_task=NULL;
static task_t * task_head[9];//task 链表的头部; 每一个cpu对应一个头部;
static int task_length[9]={0,0,0,0,0,0,0,0,0};
static task_t * current_task[9];//当前的进程;
//static int task_length=0;
static const int /*_non=0,*/_runningable=1,_running=2,_waiting=3;
//0 没有初始化 1 runningable 2 running 3 waiting 

//static inline void panic(const char *s) { printf("%s\n", s); _halt(1); }
static void kmt_init(){
  //current_task=NULL;
  //printf("In kmt_init\n");
  for(int i=0;i<9;i++)
  {
    task_head[i]=NULL;
    current_task[i]=NULL;
  }
  //int task_length=0;
  kmt_spin_init(&sem_lock,"sem_lock");
  kmt_spin_init(&task_lock,"task_lock");
  kmt_spin_init(&context_lock,"context_lock");
 
  os->on_irq(INT8_MIN, _EVENT_NULL, kmt_context_save); // 总是最先调用
  os->on_irq(INT8_MAX, _EVENT_NULL, kmt_context_switch); // 总是最后调用
   kmt_create_init(pmm->alloc(sizeof(task_t)), "cpu_0 yield", cpu_task, NULL,0);
   kmt_create_init(pmm->alloc(sizeof(task_t)), "cpu_1 yield", cpu_task, NULL,1);
   kmt_create_init(pmm->alloc(sizeof(task_t)), "cpu_2 yield", cpu_task, NULL,2);
   kmt_create_init(pmm->alloc(sizeof(task_t)), "cpu_3 yield", cpu_task, NULL,3);
 // printf("before out of kmt_init");
    //TO BE DONE
    return;
}
static _Context *kmt_context_save(_Event ev, _Context *context){
  //kmt_spin_lock(&context_lock);
  kmt_spin_lock(&task_lock);
    //TRACE_ENTRY;
  //printf("in kmt_save\n");
  if(current_task[(int)_cpu()]==NULL){
    Log1("in %d, NULL!!!",(int)_cpu());
    //kmt_spin_unlock(&task_lock);
    //_yield();
    //kmt_spin_lock(&task_lock);
    //kmt_context_save(ev,context);
    /*task_t *now=task_head[(int)_cpu()];
    while(now->next!=NULL){
      now=now->next;
    }
    current_task[(int)_cpu()]=now;*///等待修改;assert(0);
  }
  else{
  current_task[(int)_cpu()]->context=*context;
  if( current_task[(int)_cpu()]->next!=NULL)
  Log1("in save: cpu:%d name:%s status:%d",(int)_cpu(), current_task[(int)_cpu()]->name, current_task[(int)_cpu()]->status);
  if(current_task[(int)_cpu()]->status==_running)
  current_task[(int)_cpu()]->status=_runningable;
  //if(current_task[(int)_cpu()]->next==NULL){
   // printf("yield save!!!!\n\n\n\n");
 // }
  //
  }

  //kmt_spin_unlock(&context_lock);
//    TRACE_EXIT;
  kmt_spin_unlock(&task_lock);

  return NULL;
  
}
static _Context *kmt_context_switch(_Event ev, _Context *context){
  //kmt_spin_lock(&context_lock);
  kmt_spin_lock(&task_lock);
    //TRACE_ENTRY;
  //printf("In switch!");
  _Context *result=NULL;
  if(current_task[(int)_cpu()]==NULL){

    task_t *now=task_head[(int)_cpu()];
    int success_hint=0;
    if(now->status==_runningable){
      current_task[(int)_cpu()]=now;
      current_task[(int)_cpu()]->status=_running;
      result=&current_task[(int)_cpu()]->context;
      success_hint=1;
    }
    else{
      while(now->next!=NULL){
        now=now->next;
        if(now->status==_runningable){
        current_task[(int)_cpu()]=now;
        current_task[(int)_cpu()]->status=_running;
        result=&current_task[(int)_cpu()]->context;
        success_hint=1;
        break;
        }  
      }
    }
    if(success_hint==0){
      panic("ALL IS RUNNING!!");
    }
  }
  else{
  // task_t *now=NULL;
  // int success_hint=0;
  // now=task_head[(int)_cpu()];
  // if(now->status==_runningable){
  //     if(current_task[(int)_cpu()]->status==_running)
  //     {current_task[(int)_cpu()]->status=_runningable;}

  //     current_task[(int)_cpu()]=now;
  //     current_task[(int)_cpu()]->status=_running;
  //     result=&current_task[(int)_cpu()]->context;
  //     success_hint=1;
  //   }
  //   else{
  //     while(now->next!=NULL){
  //       now=now->next;
  //       if(now->status==_runningable){
  //       if(current_task[(int)_cpu()]->status==_running)
  //     {current_task[(int)_cpu()]->status=_runningable;}
  //       current_task[(int)_cpu()]=now;
  //       current_task[(int)_cpu()]->status=_running;
  //       result=&current_task[(int)_cpu()]->context;
  //       success_hint=1;
  //       break;
  //       }  
  //     }

  //     if(success_hint==0&&now->next==NULL){
  //       current_task[(int)_cpu()]=now;
  //       current_task[(int)_cpu()]->status=_running;
  //       result=&current_task[(int)_cpu()]->context;
  //     }}
  //上面是从头遍历
    task_t *now=NULL;
    int success_hint=0;
    now=current_task[(int)_cpu()];
    
    if(now->next==NULL){
      now=task_head[(int)_cpu()];
  }
    else{
      now=now->next;
    }
    task_t * test=now;
    task_t *temptask=current_task[(int)_cpu()];

    if(now->status==_runningable){
      if(current_task[(int)_cpu()]->status==_running)
        {current_task[(int)_cpu()]->status=_runningable;}
      current_task[(int)_cpu()]=now;
      current_task[(int)_cpu()]->status=_running;
      result=&current_task[(int)_cpu()]->context;
      success_hint=1;
    }
    else{
      while(now->next!=NULL){
        now=now->next;
        if(now->status==_runningable){
          if(current_task[(int)_cpu()]->status==_running)
        {current_task[(int)_cpu()]->status=_runningable;}
      current_task[(int)_cpu()]=now;
      current_task[(int)_cpu()]->status=_running;
      result=&current_task[(int)_cpu()]->context;
      success_hint=1;
      break;
        }
      }
      if(success_hint!=1){
        now=task_head[(int)_cpu()];
        while(now!=test){
          if(now->status==_runningable){
          if(current_task[(int)_cpu()]->status==_running)
        {current_task[(int)_cpu()]->status=_runningable;}
      current_task[(int)_cpu()]=now;
      current_task[(int)_cpu()]->status=_running;
      result=&current_task[(int)_cpu()]->context;
      success_hint=1;
      break;
        }
        now=now->next;
        }
      }
      if(success_hint!=1){
        while(now->next!=NULL){
          now=now->next;
        }
        current_task[(int)_cpu()]=now;
        current_task[(int)_cpu()]->status=_running;
        result=&current_task[(int)_cpu()]->context;

      }
    }
      if(temptask!=current_task[(int)_cpu()]&&temptask->alive==0){
        if(temptask->alive==0){
  
          pmm->free(temptask->stack.start);
          int success_find=0;
          task_t *now_task=NULL;
          for(int i=0;i<9;i++)
          { now_task=task_head[i];
            if(task_head[i]==NULL)continue;
            else if(task_head[i]==temptask){
              task_head[i]=now_task->next;
              success_find=1;
          }
        else{
          while(now_task->next!=NULL){
            if(temptask==now_task->next){
              task_t *find=now_task->next;
              now_task->next=find->next;
              success_find=1;
              break; 
            }
           now_task=now_task->next;
          } 
        }
      } 

        if(success_find==0)
      {
       panic("In kmt_teardown, can't find the task!!");
      }
    }
  }

  Log1("sdsdsd current_task[%d]: %s status:%d\n",(int)_cpu(),current_task[(int)_cpu()]->name,current_task[(int)_cpu()]->status);
  }
  ((proc_t*)(current_task[_cpu()]->proc))->schedule_time+=1;

 task_t *temp=task_head[(int)_cpu()];
 Log1("temp: cpu: %d name:%s status:%d",(int)_cpu(),temp->name,temp->status);
while(temp->next!=NULL){
 temp=temp->next;
Log1("temp: name:%s status:%d",temp->name,temp->status);

}
/*  if(current_task[(int)_cpu()]->next!=NULL)
  {
  printf("\n");
    Log2("current_task[%d]: %s status:%d",(int)_cpu(),current_task[(int)_cpu()]->name,current_task[(int)_cpu()]->status);
  task_t *temp=task_head[(int)_cpu()];
 Log2("temp: cpu: %d name:%s status:%d",(int)_cpu(),temp->name,temp->status);
while(temp->next!=NULL){
 temp=temp->next;
Log2("temp: name:%s status:%d",temp->name,temp->status);

}
  
  
  }*/
  Log1("head_task[%d]: %s status:%d\n",(int)_cpu(),task_head[(int)_cpu()]->name,task_head[(int)_cpu()]->status);

  if(result==NULL){
    Log1("task_list_head[%d]->status %d\n",(int)_cpu(),task_head[(int)_cpu()]->status);
    
    panic("In switch result==NULL!!");
  }
 // printf("out of switch!\n");
//    TRACE_EXIT;
  //kmt_spin_unlock(&context_lock);
  kmt_spin_unlock(&task_lock);
  return result;
}
//四个假进程,仅仅用来yield;
static int kmt_create_init(task_t *task, const char *name, void (*entry)(void *arg), void *arg,int cpu){
    //TRACE_ENTRY;
    //TO BE DONE
    kmt_spin_lock(&task_lock);
    //------------原子操作------------------ 
    Log1("Before allocate in create\n");
    Log1("create in cpu %d,name:%s",(int)_cpu(),name);
   // printf("Before allocate in create");
    task->stack.start=pmm->alloc(MAX_STACK_SIZE);
    //Log1("finish task start alloc");
    task->stack.end=task->stack.start + MAX_STACK_SIZE;
    task->status=_runningable;
    task->alive=1;
    task->name=name;
    task->context=*_kcontext(task->stack, entry, arg);//上下文上吧; 在am.h以及cte.c里面有定义;
    task->proc=procfs_add(name);

    task_t * new_task=task;
    assert(task_head[cpu]==NULL);
    //c--------head-->a-->b-->NULL-->>>>head-->c-->a-->b-->NULL
    if(task_head[cpu]==NULL)
    {
      new_task->next=NULL;
      task_head[cpu]=new_task;
    }
    task_length[cpu]+=1;
  Log1("head_task[%d]: %s status:%d\n",(int)_cpu(),task_head[(int)_cpu()]->name,task_head[(int)_cpu()]->status);
   
    //-------------原子操作-----------------
    kmt_spin_unlock(&task_lock);
//    TRACE_EXIT;
    return 0;



}
static void cpu_task(void *arg){
  /*if((int)_cpu()==0){*/
    while(1){
      _yield();
   }
  //}
};
/*static void cpu1_task(void *arg){
  if((int)_cpu()==1){
    while(1){
      _yield();
    }
  }
};
static void cpu2_task(void *arg){
  if((int)_cpu()==2){
    while(1){
      _yield();
    }
  }
};
static void cpu3_task(void *arg){
  if((int)_cpu()==3){
    while(1){
      _yield();
    }
  }
};
*/

static int kmt_create(task_t *task, const char *name, void (*entry)(void *arg), void *arg){
  //  TRACE_ENTRY;
    //TO BE DONE
    kmt_spin_lock(&task_lock);
    //------------原子操作------------------ 
    Log1("Before allocate in create\n");
    Log1("create in cpu %d,name:%s",(int)_cpu(),name);
   // printf("Before allocate in create");
    task->stack.start=pmm->alloc(MAX_STACK_SIZE);
    //Log1("finish task start alloc");
    task->stack.end=task->stack.start + MAX_STACK_SIZE;
    task->status=_runningable;
    task->name=name;
    task->alive=1;
    task->context=*_kcontext(task->stack, entry, arg);//上下文上吧; 在am.h以及cte.c里面有定义;
    task->proc=procfs_add(name);
    Log1("create: name:%s\tstatus:%d\n",task->name,task->status);
    task_t * new_task=task;

    //c--------head-->a-->b-->NULL-->>>>head-->c-->a-->b-->NULL
    int least=0x3f3f3f3f;
    int least_cpu=0;
    if(task_length[0]>=4){
    for(int i=0;i<_ncpu();i++){
      if(task_length[i]<least){least=task_length[i];least_cpu=i;}
    }}
    task_length[least_cpu]+=1;

    assert(task_head[(int) _cpu()]!=NULL);
    /*if(task_head[(int)_cpu()]==NULL)
    {
      new_task->next=NULL;
      task_head[(int)_cpu()]=new_task;

      assert(0);
    }
    else
    {
      new_task->next=task_head[(int)_cpu()];//此时head是a
      task_head[(int)_cpu()]=new_task;//把头变成c;
    }
*/    if(task_head[least_cpu]==NULL)
    {
      new_task->next=NULL;
      task_head[least_cpu]=new_task;

      assert(0);
    }
    else
    {
      new_task->next=task_head[least_cpu];//此时head是a
      task_head[least_cpu]=new_task;//把头变成c;
    }
    //-------------原子操作-----------------
    kmt_spin_unlock(&task_lock);
  //  TRACE_EXIT;
    return 0;
}

static void kmt_teardown(task_t *task){
    //  从head 数组开始遍历,找到它然后释放;
    //TO BE DONE
  //  TRACE_ENTRY; 


  //没有修改task_length!!可能会产生问题;
    kmt_spin_lock(&task_lock);
    //------------原子操作------------------ 
    printf("In kmt_treardown\n");
    if(task->status==_runningable){
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
    }}
    else{
      task->alive=0;
    }
    //-------------原子操作-----------------
    kmt_spin_unlock(&task_lock);
  //  TRACE_EXIT;
  //如果他runningable--->删掉;
  //如果他waiting--->alive=0;在switch里面runningable的时候他杀
  //如果running----->alive=0;在switch里面runningable的时候他杀
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
  //kmt_spin_init(sem->lock);  
  sem->end=0;
  sem->start=0;//当head%MAXSIZE与tail%MAXSIZE相等的时候,队列是空的;
                          //当(tail+1)%MAXSIZE与head%MAXSIZE相等的时候,队列是满的;
    // printf("int init: name:%s\n\n",sem->name); 
  return ;
}

static void kmt_sem_wait(sem_t *sem){
  kmt_spin_lock(&sem_lock);
//  kmt_spin_lock(&task_lock);
  //------------原子操作------------------ 
 
  if(current_task[(int)_cpu()]==NULL){
    panic("In sem_wait No task in this cpu");
  }
  //---------对于每一个sem: tasklist:
  //----先进先出；参考数据结构:https://blog.csdn.net/a04081122/article/details/51985873
  while(sem->value<=0){
    current_task[(int)_cpu()]->status=_waiting;
    //sem->end++;
    if(((sem->end+1)%sem->MAXSIZE)==(sem->start%sem->MAXSIZE))
    {
      // for(int i=0;i<sem->MAXSIZE;i++)
      // {
      //   printf("%d %s status: \n",i,sem->task_list[i]->name,sem->task_list[i]->status);
      // }
     //printf("name:%s\n\n",sem->name); 
      panic("In sem_wait, the task_list is full;");}
    //int if_sleep;
    sem->task_list[sem->end]=current_task[(int)_cpu()];
    sem->end++;
    sem->end%=sem->MAXSIZE;
    
    kmt_spin_unlock(&sem_lock);
    _yield();
    kmt_spin_lock(&sem_lock);
    //理论上不可能发生进入两次队列的情况 如果后面有bug可以在这个地方加一个assert;
  }
   sem->value--;
  //------------原子操作------------------ 
  kmt_spin_unlock(&sem_lock);
  //kmt_spin_unlock(&task_lock);

  return;
}

static void kmt_sem_signal(sem_t *sem){
  //TRACE_ENTRY;
  kmt_spin_lock(&sem_lock);
//  kmt_spin_lock(&task_lock);
  //------------原子操作------------------ 
  sem->value++;
  Log1("value:%d",sem->value);
  if(sem->start%sem->MAXSIZE==sem->end){
    assert(sem->value>0);//队列为空;
  }else
  {
    sem->start%=sem->MAXSIZE;
    /*printf("in semi signal: semi->name:%s,task->name:%s status:%d\n",sem->name,sem->task_list[sem->start]->name,sem->task_list[sem->start]->status); 
     for(int i=sem->start%sem->MAXSIZE;i<sem->end;i=(i+1)%sem->MAXSIZE){
      printf("in semi-signal[%d] name:%s\n",i,sem->task_list[i]->name);
    }
    printf("\n");*/
    sem->start%=sem->MAXSIZE; 
    if(sem->task_list[sem->start]==NULL)panic("In sem_signal task_list meeting NULL");
    if(sem->task_list[sem->start]->status==_waiting)
    {sem->task_list[sem->start]->status=_runningable;}
    else{
      assert(0);
    }
     
    sem->start+=1;
    sem->start%=sem->MAXSIZE;
   /* printf("in semi signal: semi->name:%s,task->name:%s status:%d\n",sem->name,sem->task_list[sem->start-1]->name,sem->task_list[sem->start-1]->status); 
     for(int i=sem->start%sem->MAXSIZE;i<sem->end;i=(i+1)%sem->MAXSIZE){
      printf("in semi-signal[%d] name:%s\n",i,sem->task_list[i]->name);
    }
    printf("\n");*/
  }
  
  //------------原子操作------------------ 
//  kmt_spin_unlock(&task_lock);
  kmt_spin_unlock(&sem_lock);
  //TRACE_EXIT;
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
