#include <common.h>
#include "x86.h"
static int ncli[9]={0,0,0,0,0,0,0,0,0};
static int intena[9]={0,0,0,0,0,0,0,0,0};
static inline void panic(const char *s) { printf("%s\n", s); _halt(1); }
static void kmt_init(){

    //TO BE DONE
    return;
}

static int kmt_create(task_t *task, const char *name, void (*entry)(void *arg), void *arg){

    //TO BE DONE


    return 0;
}

static void kmt_teardown(task_t *task){

    //TO BE DONE

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
     printf("%d %d , ", lk->locked,lk->cpu); 
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
  lk->cpu = 0;

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


    return ;
}

static void kmt_sem_wait(sem_t *sem){


    return;
}

static void kmt_sem_signal(sem_t *sem){


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
