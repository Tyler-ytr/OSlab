#ifndef __COMMON_H__

#include <kernel.h>
#include <nanos.h>
struct pthread_t
{
  intptr_t lock;
};

extern intptr_t my_spin_lock(struct pthread_t  *locked);
extern intptr_t my_spin_unlock(intptr_t locked); 

//#define pthread_t intptr_t

#endif
