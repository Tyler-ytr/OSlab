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
  //双向链表,注意更新 node 的 next,prev,num,si;
  //注意更新void* max;
  //尝试一下合并


  _list now=head;
  while(now->next!=head)
  {
    now=now->next;
    if(now->flag==0&&now->size>=size)
    {
      break;
    }
  }
//退出来的时候:now:1.可能是不符合要求的尾节点;2.可能是符合要求的尾节点;3.可能是符合要求的中间节点

  if(now->size<size&&now->next==head)
  {
      _list new=(void *)(&now[1]+now->size);
      now->next=new;
      new[0].prev=now;
      assert(head==now->next);
      new[0].next=head;
      new[0].addr=&new[1];
      new[0].flag=1;
      new[0].size=size;
      ret=new[0].addr;
      assert(new->prev->next==new);
      printf("new:area 0x%x",&new[0]);

  }
  else{
    //尝试一下拆分节点:
    int minsize=sizeof(_node);
    if(now->size-size>minsize)//可以拆分节点;
    {
      _list new=(void *)(&now[1]+size);
      printf("拆分中:now_area: 0x%x, now_size: 0x%x size :0x%x new_area:0x%x\n",&now[0],now->size,size,&new[0]);

      new->prev=now;
      assert(new[0].prev==new->prev);
      new->next=now->next;
//      new->prev=now;

      new->addr=&new[1];
      new->size=now->size-size-sizeof(_node);
      new->flag=0;
      assert(new->size<=0);
      now->next=new;
      now->size=size;
      now->flag=1;
      ret=now[0].addr;
      assert(new->prev->next==new);
      assert(now->prev->next==now);
    }else//不够拆分节点;
    {
      now->flag=1;
      ret=now[0].addr;


    }



    
  }

  
  





  if(head->size!=0)
  {
    printf("BUG: head->size has changed!\n");
      assert(0);
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
