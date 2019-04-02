#include <common.h>
#include <klib.h>

static uintptr_t pm_start, pm_end;
//static uintptr_t start;
void *max;
static void pmm_init() {
  pm_start = (uintptr_t)_heap.start;
  printf("start:0x%x",pm_start);
  pm_end   = (uintptr_t)_heap.end;
  printf("end:0x%x\n",pm_end);
  //start=pm_start;
  max=(void *)pm_start;
  
  test.next=&test;
  test.prev=&test;
  test.addr=max;
  test.size=0;
  test.flag=1;

  bound*b1=max;
  b1[0].left_bound=&b1[1];
  
  head=(void*)&b1[1];
  b1[0].using_one=&head[1];
  b1[0].right_bound=&head[17];

  printf("\nb1:0x%x,b1[0]:0x%x,b1[0].left_bound:0x%x;b1[0].using_one:0x%x,b1[1]:0x%x\n",&b1,&b1[0],b1[0].left_bound,b1[0].using_one,&b1[1]);


  head->next=head;
  head->prev=head;
  head->addr=b1[0].right_bound;
  head->size=0;
  head->flag=1;
  printf("head_place:0x%x,head->next：0x%x",&head,head->next);
  printf("test_place:0x%x,test.next：0x%x",&test,test.next);
  
}

static void *kalloc(size_t size) {
 //pthread_t alloc_lock=0; 
 //my_spin_lock(alloc_lock);
/* printf("start1: 0x%x",start);
 start+=size;
 printf("start2: 0x%x",start);
 void *ret=&start; 
 printf(" ret:0x%x 0x%x\n",ret,*(int *)ret);
 //my_spin_unlock(alloc_lock);
  */

  pthread_t alloc_lock=0;
  my_spin_lock(alloc_lock);
  void *ret=NULL;





  my_spin_unlock(alloc_lock);
  return ret;
}

static void kfree(void *ptr) {
}

MODULE_DEF(pmm) {
  .init = pmm_init,
  .alloc = kalloc,
  .free = kfree,
};
