#include <common.h>
//#include <klib.h>

//static spinlock init_lk;
//static spinlock alloc_lk;
spinlock_t alloc_lk;
spinlock_t free_lk;
static spinlock head_lk;
static uintptr_t pm_start, pm_end;
extern uint64_t total_memory;
extern uint64_t used_memory_reality;
static void pmm_init() {
  //spinlock*lk=&init_lk;
  //initlock(lk,NULL);
  //lock(lk);

  //spinlock *a_lk=&alloc_lk;
  spinlock_t *a_lk=&alloc_lk;
  spinlock_t *f_lk=&free_lk;
   spinlock *h_lk=&head_lk;
    //initlock(a_lk,NULL);
    kmt->spin_init(a_lk,"alloc");
    kmt->spin_init(f_lk,"free");

    initlock(h_lk,NULL);

  pm_start = (uintptr_t)_heap.start;
  //printf("start:0x%x",pm_start);
  pm_end   = (uintptr_t)_heap.end;
  //printf("end:0x%x\n",pm_end);
  total_memory=pm_end-pm_start;
  used_memory_reality=1;

  unused_space=(void *)pm_start;
  unused_space->next=unused_space;
  unused_space->prev=unused_space;
  unused_space->flag=3;
  unused_space->size=0;

  cpu_head[0]=&unused_space[1];
  cpu_head[0]->next=cpu_head[0];
  cpu_head[0]->prev=cpu_head[0];
  cpu_head[0]->flag=2;
  cpu_head[0]->size=0;
  for (int i=1;i<=8;i++)
  {
    cpu_head[i]=&(cpu_head[0])[i];
    cpu_head[i]->next=cpu_head[i];
    assert(cpu_head[i]->next!=NULL);
    printf("i : %d cpu_head ->next :0x%x",i,cpu_head[i]->next);
    cpu_head[i]->prev=cpu_head[i];
    cpu_head[i]->flag=2;
    cpu_head[i]->size=0;
  }

  printf("cpu_area: 0x%x, 1: 0x%x ; 4: 0x%x \n",cpu_head[0],cpu_head[1],cpu_head[4]);
  unused_space->addr=&(cpu_head[0])[9];
  printf("first_area: 0x%x \n",unused_space->addr);

//  unlock(lk);
}

static void *kalloc(size_t size) {

  //spinlock*a_lk=&alloc_lk;
  spinlock_t*a_lk=&alloc_lk;
  //lock(a_lk);
  kmt->spin_lock(a_lk);
   int cpu_num=(int)_cpu();
  _list head=cpu_head[cpu_num];
  _list now=cpu_head[cpu_num];
  void *ret=NULL;
  int success_hint=0;
  if(size==0)
  {
    ret=(void *)pm_start;
  }
  else{
  while(now->next!=head)
  {
    now=now->next;
    assert(now!=NULL);
    if(now->flag==0&&now->size>=size)
    {
      success_hint=1;//表示当前的遍历到的节点可以使用
      break;
    }
  }

  if(success_hint!=1)
  {
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
    if(unused_space->addr>(void*)pm_end)
    {
      printf("Your memory is filled. I am sorry!");
      assert(0);
    }

    assert(now->next->prev==now);
    assert(new->next->prev==new);
    assert(now->prev->next==now);
    assert(new->prev->next==new);
    assert(now->next!=NULL);
    assert(new->next!=NULL);

    assert(new->addr!=NULL);
    assert(unused_space->addr!=new);
    ret=new->addr;
  }
  else
  {//下面的操作是拆分或者直接使用,所以不用修改unused_space；

    if((int)(now->size-size-2*sizeof(_node))-4028>0&&size>4028)
    {
      assert((int)(now->size-size)>sizeof(_node));
      //返回的是now的地址;
      if(now->size==0)
      assert(0);
      _list new=(void *)(now->addr+size);


      new->next=now->next;

      assert(now->next->prev==now);
      now->next->prev=new;
      new->next->prev=new;
      new->prev=now;
      new->addr=&new[1];
      new->flag=0;
      new->size=now->size-size-sizeof(_node);

      now->next=new;
      now->size=size;
      now->flag=1;
      ret=now->addr;
      assert(now->next->prev==now);

      if(new->next->prev!=new){
        printf("new: 0x%x  -----: 0x%x  size:%d  new_size:%d\n",new,new->next->prev,size,new->size);
        assert(0);
      }
      assert(now->prev->next==now);
      assert(new->prev->next==new);
    }

    else
    {
      now->flag=1;
      ret=now->addr;
    }
  }
  }
  assert(ret!=NULL);
  //unlock(a_lk);
  printf("In alloc size: %d\n",size);
 // used_memory_reality+=size;
   //     printf("used memory:%d\n",used_memory_reality);
  kmt->spin_unlock(a_lk);
  return ret;
}

static void kfree(void *ptr) {

  //spinlock*a_lk=&alloc_lk;
  spinlock_t *f_lk=&free_lk;
  //lock(a_lk);
  kmt->spin_lock(f_lk);

  //首先搜索这个地址的存在性；
  if(ptr!=NULL&&ptr!=(void *)pm_start)
  {
      int success_hint=0;
      _list head=NULL;
      _list now=NULL;
      for(int i=0;i<=8;i++)
      {
         head=cpu_head[i];
         now=cpu_head[i];
        while(now->next!=head)
        {
          now=now->next;
          if(now->addr==ptr)
          {
            success_hint=1;
            break;
          }
        }
        if(success_hint==1)
        {
          break;
        }
      }
      if(success_hint==1)
      {
        printf("free :0x%x\n",ptr);
        if(now->flag==2)
        assert(0);
        now->flag=0;
        used_memory_reality-=now->size;
        printf("used memory:%d\n",used_memory_reality);
      }

  }
  printf("in free\n");
  kmt->spin_unlock(f_lk);


}


MODULE_DEF(pmm) {
  .init = pmm_init,
  .alloc = kalloc,
  .free = kfree,
};
