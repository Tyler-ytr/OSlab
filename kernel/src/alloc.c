#include <common.h>
#include <klib.h>

static spinlock init_lk;
static spinlock alloc_lk;
static spinlock head_lk;
static uintptr_t pm_start, pm_end;
//static uintptr_t start;
static void pmm_init() {
  spinlock*lk=&init_lk;
  initlock(lk,NULL);
  lock(lk);

  spinlock *a_lk=&alloc_lk;
   spinlock *h_lk=&head_lk;
    initlock(a_lk,NULL);
    initlock(h_lk,NULL);

  pm_start = (uintptr_t)_heap.start;
  printf("start:0x%x",pm_start);
  pm_end   = (uintptr_t)_heap.end;
  printf("end:0x%x\n",pm_end);

  unused_space=(void *)pm_start;
  unused_space->next=unused_space;
  unused_space->prev=unused_space;
  unused_space->flag=3;
  unused_space->size=0;

  cpu_head[0]=&unused_space[1];
  cpu_head[1]=&(cpu_head[0])[1];
  for (int i=1;i<=4;i++)
  {
    cpu_head[i]=&(cpu_head[0])[i];
    cpu_head[i]->next=cpu_head[i];
    cpu_head[i]->prev=cpu_head[i];
    cpu_head[i]->flag=2;
    cpu_head[i]->size=0;
  }    

  printf("cpu_area: 0x%x, 1: 0x%x ; 4: 0x%x \n",cpu_head[0],cpu_head[1],cpu_head[4]);
  unused_space->addr=&(cpu_head[0])[5];
  printf("first_area: 0x%x \n",unused_space->addr);

  unlock(lk);
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

 // pthread_t alloc_lock=0;
//  my_spin_lock(alloc_lock);


  //我觉得为了防止同时使用表头,应该锁住它

//  spinlock *h_lk=&head_lk;
  //lock(h_lk);
  //int cpu_num=_cpu();
  //_list head=cpu_head[cpu_num];
  //_list now=cpu_head[cpu_num];  
  //unlock(h_lk);

 // void *ret=NULL;
  spinlock*a_lk=&alloc_lk;
  lock(a_lk);
  printf("In alloc, after lock a_lk\n");
   int cpu_num=_cpu();
  _list head=cpu_head[cpu_num];
  _list now=cpu_head[cpu_num]; 
  void *ret=NULL;
  int success_hint=0;
  while(now->next!=head)
  { 
    now=now->next;
    if(now->flag==0&&now->size>=size)
    {
      success_hint=1;//表示当前的遍历到的节点可以使用
      break;
    }
  }

  if(success_hint!=1)
  {
    printf("cpu_num :%d",_cpu());
    printf("success: %d\n",success_hint);
    assert(head==now->next);
    _list new=(void*)unused_space->addr;//记得更新unused->space;
    new->next=now->next;
    now->next->prev=new;
    new->prev=now;
    new->addr=&new[1];
    new->flag=1;
    new->size=size;
    
    now->next=new;
    unused_space->addr=(void *)&new[1]+size;//一定保护好unused_space
    printf("unused_space->addr: 0x%x\n",unused_space->addr);

    assert(now->next->prev==now);
    assert(new->next->prev==new);
    assert(now->prev->next==now);
    assert(new->prev->next==new);

    assert(new->addr!=NULL);
    assert(unused_space->addr!=new);
    ret=new->addr;
    printf("OVER!!!!!!\n");
  }
  else
  {//下面的操作是拆分或者直接使用,所以不用修改unused_space；
    if((int)(now->size-size-sizeof(_node))>0)
    {
      assert((int)(now->size-size)>sizeof(_node));
      //返回的是now的地址;
      _list new=(void *)(now->addr+size);
      assert(&new[1]+size==now->addr+size);
    
      new->next=now->next;
      
      now->next->prev=new;
      new->prev=now;
      new->addr=&new[1];
      new->flag=0;
      new->size=now->size-size;

      now->next=new;
      now->size=size;
      now->flag=1;
      ret=now->addr;
      assert(now->next->prev==now);
      assert(new->next->prev==new);
      assert(now->prev->next==now);
      assert(new->prev->next==new);
    }
      
    else
    {
      now->flag=1;
      ret=now->addr;
    }
  }
  printf("cpu :%d ret: 0x%x\n",cpu_num,ret);
  assert(ret!=NULL);
  unlock(a_lk);
  return ret;
  //首先遍历整个链表,如果存在flag==0并且size足够大的节点,就选它,返回addr，如果没有就创建一个新的节点,此时需要记得更新！！！！;
  //双向链表,注意更新 node 的 next,prev,num,si;
  //注意更新void* max;
  //尝试一下合并

/*
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
      assert(head==now->next);
      _list new=(void *)(&now[1]+now->size);
      now->next=new;
      new[0].prev=now;
      new[0].next=head;
      new[0].addr=&new[1];
      printf("new [1]_area:0x%x",&new[1]);
      new[0].flag=1;
      new[0].size=size;
      ret=new[0].addr;
      assert(new->prev->next==new);
      printf("new:area 0x%x",&new[0]);

  }
  else{
    //尝试一下拆分节点:
    //int minsize=sizeof(_node);
    printf("result one before if : %d \n",now->size-size-sizeof(_node));
    if((int)(now->size-size-sizeof(_node))>0)//可以拆分节点;
    {
      printf("now->prev;0x%x now_addr0x%x now->prev->next:0x%x\n",now->prev,&now[0],now->prev->next);
      _list new=(void *)(&now[1]+size);
      printf("拆分中:now_area: 0x%x, now_size: %d size :%d sizeofnode:%d,new_area:0x%x\n",&now[0],now->size,size,sizeof(_node),&new[0]);

      new->prev=now;
      assert(new[0].prev==new->prev);
      new->next=now->next;
//      new->prev=now;

      new->addr=&new[1];
      new->size=now->size-size-sizeof(_node);
      new->flag=0;
      printf("new_size:%d",new->size);
      assert(new->size>=0);
      now->next=new;
      printf("now->prev;0x%x now_addr0x%x now->prev->next:0x%x\n",now->prev,&now[0],now->prev->next);
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
  assert((int)ret<pm_end);
  printf("ret:0x%x",ret);
// my_spin_unlock(alloc_lock);
  return ret;*/
}

static void kfree(void *ptr) {
// free 主要要注意合并节点的问题





;








}

MODULE_DEF(pmm) {
  .init = pmm_init,
  .alloc = kalloc,
  .free = kfree,
};
