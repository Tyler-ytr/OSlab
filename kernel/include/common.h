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
  int flag;//0,1 用来标示这块用了没,2表示这是head;
  int size;// 单位是byte
  int num;//用来反推bound的位置;
}_node,*_list;

typedef struct BOUND{
 struct BOUND* prev;
 struct BOUND* next;
 void *left_bound;//记录bound块的右边缘,_node块的左边缘,是_node块的起始位置;
 void *right_bound;//记录目前_node块的右边缘,是实际数据区的起始位置
 void *using_one;//记录目前正在使用的_node 的右边界,using_one应该是下个_node的起始位置;
}bound;// 我准备在堆区动态的建立存放链表,bound用来指这块地方的边界以及正在使用的链表的位置；

//|bound_size| 16*_node_size| 16 block size |bound_size|16*node_size|16_block_size|

_list head;
void* max;
//_node test;




#endif
