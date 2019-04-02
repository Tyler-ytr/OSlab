#ifndef __COMMON_H__

#include <kernel.h>
#include <nanos.h>

extern intptr_t my_spin_lock(intptr_t locked);
extern intptr_t my_spin_unlock(intptr_t locked); 

#define pthread_t intptr_t

typedef struct _LIST_for_alloc
{
  struct _LIST_for_alloc *next;
  struct _LIST_for_alloc *prev;
  void * addr;
  int flag;// 用来标示这块用了没
  int size;// 单位是byte

}_node,*_list;

typedef struct BOUND{
 BOUND* prev;
 BOUND* next;
 void *left_bound;
 void *right_bound;
 void *using_one;
}bound;// 我准备在堆区动态的建立存放链表,bound用来指这块地方的边界以及正在使用的链表的位置；


_list head;




#endif
