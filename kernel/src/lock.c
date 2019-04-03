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

int ncli[5]={0,0,0,0,0};
int intena[5]={0,0,0,0,0};
void initlock(struct Spinlock *lk,char *name)
{
  lk->name=name;
  lk->locked=0;
  lk->cpu=0;
  //memset(ncli,0,sizeof(ncli));
  //memset(intena,0,sizeof(intena));
}


void pushcli(int cpu_num)
{
    /*int eflags;

      eflags = readeflags();
        cli();
        printf("in pushcli of cpu :%d",_cpu());
          if(ncli[_cpu()] == 0)
                intena[_cpu()] = eflags & FL_IF;
            ncli[_cpu()] += 1;
            printf(" ncli: %d\n",ncli[_cpu()]);
*/
    cli();
   // printf("push ncli: cpu %d ncli[_cpu]: %d\n",_cpu(),ncli[(int)_cpu()]);
   // int cpu_num=_cpu();
    ncli[cpu_num]+=1;
}

void popcli(int cpu_num)
{
   /* if(readeflags()&FL_IF)
          {panic("popcli - interruptible");
          assert(0);}
          
        printf("in popcli of cpu :%d",_cpu());
         // printf("in popcli of cpu :%d ncli before -1:%d \n",cpu_num,ncli[cpu_num]);
      if(--ncli[_cpu()] < 0)
            {panic("popcli, cpu: %d ncli: %d\n",_cpu(),ncli[_cpu()]);
            assert(0);}
          printf("in popcli of cpu :%d ncli before :%d \n",_cpu(),ncli[_cpu()]);
        if(ncli[_cpu()] == 0 && intena[_cpu()])
              sti();
*/
 // int cpu_num=_cpu();
  ncli[cpu_num]--;
    //printf("pop ncli: cpu %d ncli[_cpu]:%d \n",_cpu(),ncli[(int)_cpu()]);
  assert(ncli[cpu_num]>=0);
  if(ncli[_cpu()]==0)
  {

    sti();
  }

}
/*
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
*//*
int
holding(struct Spinlock *lock)
{
    int r;
      pushcli();
        r = lock->locked && lock->cpu == (int)_cpu();
          popcli();
            return r;

}
*/

void lock(struct Spinlock *lk)
{     lk->cpu = _cpu();
    pushcli(lk->cpu); // disable interrupts to avoid deadlock.
    /*  if(holding(lk))
            {panic("acquire");
            assert(0);}
*/
        // The xchg is atomic.
           while(xchg(&lk->locked, 1) != 0)
               ;
        
        printf("In lock \n");
                 // Tell the C compiler and the processor to not move loads or stores
                   // past this point, to ensure that the critical section's memory
                     // references happen after the lock is acquired.
                       //__sync_synchronize();
        
                        // Record info about lock acquisition for debugging.
                      
                            // getcallerpcs(&lk, lk->pcs);
        
}
void unlock(struct Spinlock *lk)
{
     //   printf("In unlock \n");
/*    if(!holding(lk))
          {panic("release");
          assert(0);}
*/
     

          // Tell the C compiler and the processor to not move loads or stores
             // past this point, to ensure that all the stores in the critical
               // section are visible to other cores before the lock is released.
                 // Both the C compiler and the hardware may re-order loads and
                   // stores; __sync_synchronize() tells them both not to.
                     //__sync_synchronize();
          
                       // Release the lock, equivalent to lk->locked = 0.
                         // This code can't use a C assignment, since it might
                          // not be atomic. A real OS would use C atomics here.
                        //     asm volatile("movl $0, %0" : "+m" (lk->locked) : );
          xchg(&lk->locked, 0) ;
                               popcli(lk->cpu);
                lk->pcs[0] = 0;
        lk->cpu = 0;                
          
}






