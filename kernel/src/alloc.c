#include <common.h>
#include <klib.h>

static uintptr_t pm_start, pm_end;
//static uintptr_t start;
static void pmm_init() {
  pthread_t init_lock=0;
  my_spin_lock(init_lock);
  pm_start = (uintptr_t)_heap.start;
  printf("start:0x%x",pm_start);
  pm_end   = (uintptr_t)_heap.end;
  printf("end:0x%x\n",pm_end);
  //start=pm_start;
  max=(void *)pm_start;
 /* 
  test.next=&test;
  test.prev=&test;
  test.addr=max;
  test.size=0;
  test.flag=1;
*/
  //bound*b1=max;
  //b1[0].left_bound=&b1[1];
  
  head=(void*)pm_start;
  //b1[0].using_one=&head[1];
  //b1[0].right_bound=&head[17];

 // printf("\nb1:0x%x,b1[0]:0x%x,b1[0].left_bound:0x%x;b1[0].using_one:0x%x,b1[1]:0x%x\n",&b1,&b1[0],b1[0].left_bound,b1[0].using_one,&b1[1]);


  head->next=head;
  head->prev=head;
  head->addr=&head[1];
  head->size=0;
  head->flag=2;
  /*void* result=(void *)&head[0]-(head->num-1)*sizeof(_node)-sizeof(bound);
  printf("result:0x%x bound_area:0x%x\n",result,sizeof(bound));*/
  printf("head_place:0x%x,head->next：0x%x,head->addr:0x%x\n",&head[0],head->next,head[0].addr);
 // printf("test_place:0x%x,test.next：0x%x",&test,test.next);
  my_spin_unlock(init_lock);
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

  //首先遍历整个链表,如果存在flag==0并且size足够大的节点,就选它,返回addr，如果没有就创建一个新的节点,此时需要记得更新！！！！;
  //双向链表,注意更新 node 的 next,prev,num,size,以及BOUND的using_one;
  //理论上,真正的尾节点的地址应该就是当前的bound的using_one;
  //注意更新void* max;
  //注意每16个node要新开一个bound,node;

  /*_list now_pnode=head;
  while(now_pnode->next!=head)
  {
    now_pnode=now_pnode->next;
    if(now_pnode->flag==0)
    {
      if(now_pnode->size>=size)
      {
        break;
      }
    }
*/
    //now_pnode=now_pnode->next;



  }

  //此时,要么出来的now_pnode符合要求可以直接返回,要么是不符合要求的尾节点,now_pnode->next=head;

  //如果是尾节点;首先需要考虑它的num是不是16;
  //
  if(now_pnode->next=head&&now_pnode->size<size)
  {
    printf("num: %d \n",now_pnode->num);
    if(now_pnode->num==16)
    {

    }
  }
  else{

//To be continued
//插入,拆分节点,快乐吗？

  }







  
  






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
