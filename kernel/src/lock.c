#include <common.h>
#include <klib.h>
//#include "types.h"
//#include "defs.h"
//#include "param.h"
#include "x86.h"
//#include "memlayout.h"
//#include "mmu.h"
//#include "proc.h"
//#include "spinlock.h"

/*
intptr_t xchg(volatile intptr_t *addr, intptr_t newval) {
  intptr_t result;
asm volatile ("lock xchgl %0, %1":
    "+m"(*addr), "=a"(result) : "1"(newval) : "cc");
return result; 
}
*/
/*
intptr_t my_spin_lock(intptr_t locked)
{

  intptr_t value=1;
  //printf("I am here");
  cli();
  while (1)
  {
    printf("old:%d\n",value);
   value=my_atomic_xchg(&locked,1);
    printf("new:%d\n",value);
   if(value==1)
    {
      break;
    }

  };
  return 0;
}

intptr_t my_spin_unlock(intptr_t locked)
{
  
  printf("I am in unlock\n");
 my_atomic_xchg(&locked, 0);
 sti();
 return 0;
}
*/


void initlock(struct Spinlock *lk,char *name)
{
  lk->name=name;
  lk->locked=0;
  lk->cpu=0;
  memset(ncli,0,sizeof(ncli));
  memset(intena,0,sizeof(intena));
}


void pushcli(void)
{
    int eflags;

      eflags = readeflags();
        cli();
        int cpu_num=_cpu();
          if(ncli[cpu_num] == 0)
                intena[cpu_num] = eflags & FL_IF;
            ncli[cpu_num] += 1;

}

void popcli(void)
{
    if(readeflags()&FL_IF)
          panic("popcli - interruptible");
          int cpu_num=_cpu();
      if(--ncli[cpu_num] < 0)
            panic("popcli");
        if(ncli[cpu_num] == 0 && intena[cpu_num])
              sti();

}
void
getcallerpcs(void *v, uint pcs[])
{
    uint *ebp;
      int i;

        ebp = (uint*)v - 2;
        for(i = 0; i < 10; i++){
              if(ebp == 0 || ebp < (uint*)KERNBASE || ebp == (uint*)0xffffffff)
                      break;
                  pcs[i] = ebp[1];     // saved %eip
                      ebp = (uint*)ebp[0]; // saved %ebp
                        
        }
          for(; i < 10; i++)
                pcs[i] = 0;

}

int
holding(struct Spinlock *lock)
{
    int r;
      pushcli();
        r = lock->locked && lock->cpu == _cpu();
          popcli();
            return r;

}


void lock(struct Spinlock *lk)
{
    pushcli(); // disable interrupts to avoid deadlock.
      if(holding(lk))
            panic("acquire");

        // The xchg is atomic.
           while(xchg(&lk->locked, 1) != 0)
               ;
        
                 // Tell the C compiler and the processor to not move loads or stores
                   // past this point, to ensure that the critical section's memory
                     // references happen after the lock is acquired.
                       __sync_synchronize();
        
                        // Record info about lock acquisition for debugging.
                           lk->cpu = _cpu();
                             getcallerpcs(&lk, lk->pcs);
        
}
void unlock(struct Spinlock *lk)
{
    if(!holding(lk))
          panic("release");

      lk->pcs[0] = 0;
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
          
}






