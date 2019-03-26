#ifndef __COMMON_H__

#include <kernel.h>
#include <nanos.h>
typedef struct pThread_t
{
  intptr_t lock;
}pthread_t;

extern intptr_t my_spin_lock(struct pthread_t  *locked);
extern intptr_t my_spin_unlock(intptr_t locked); 

//#define pthread_t intptr_t

#endif
