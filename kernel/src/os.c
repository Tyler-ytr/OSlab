#include <common.h>
#include <klib.h>
spinlock_t lk_irq;
spinlock_t lk_test;
static Handler_list handler_list[MAX_HANDLIST_NUMBER];
static int _handler_length=0;


// void test_from_yzy(){
//      // void *space[500];
//           /*for(int j=0;i<=7;j++){
//           for(i=0;i<500;++i){
//                     space[i]=pmm->alloc(rand()%((1<<10)-1)+4028);
//                     printf("i: %d space:0x%x\n",i,space[i]);

//           }}
//           for(i=0;i<5000;++i){
//                     int temp=rand()%10;
//                             pmm->free(space[temp]);
//                                     space[temp]=pmm->alloc(rand()&((1<<10)-1));
//                     printf("temp: %d space:0x%x\n",temp,space[temp]);

//           }
//           for(i=0;i<500;++i){
//                     pmm->free(space[i]);

//           }
// */        
//          int *arr = (int*)pmm->alloc(20 * sizeof(int));
//          for (int i = 0; i < 20; i++){
//     arr[i] = i;
//   }
//   for (int i = 0; i < 20; i++){printf("arr[%d]: %d", i, arr[i]);}
//           printf("success!!!!!!");

// }

static void os_init() {
 // printf("%d\n\n\n\n\n",(int)_cpu());
  pmm->init();
  //printf("%d\n\n\n\n\n",(int)_cpu());
  kmt->spin_init(&lk_irq,"/src/os os_on_irq lock");
  kmt->spin_init(&lk_test,"/src/os test");
  
  //To be continued:
  kmt->init();
  //printf("%d\n\n\n\n\n",(int)_cpu());
//  _vme_init(pmm->alloc, pmm->free);
  //printf("before dev\n");
  //assert(0);
  dev->init();
  vfs->init();

//  printf("after init");
  // 创建你的线程，线程可以调用`tty->ops->read`或`tty->ops->write`/


}

static void hello() {
  for (const char *ptr = "Hello from CPU #"; *ptr; ptr++) {
    _putc(*ptr);
  }
  _putc("12345678"[_cpu()]); _putc('\n');
}

static void os_run() {
//spinlock *lk=NULL;
 //spinlock * lk=&os_lk;
  //initlock(lk,NULL);
  //lock(lk);
  //int locked=0;
  //my_spin_lock(locked);
  kmt->spin_lock(&lk_test);
  hello();
  //test_from_yzy();
  kmt->spin_unlock(&lk_test);
  //test_from_yzy();
  //void *p=NULL;
  //_yield();

 // p=pmm->alloc(1000);
  //printf("p:0x%x\n",p);
  //my_spin_unlock(locked);
  //unlock(lk);
  _intr_write(1);
  while (1) {
    _yield();
  }
}

static _Context *os_trap(_Event ev, _Context *context) {
  _Context *ret=NULL;
 // _Context *ret=context;//记得修改
  for(int i=0;i<_handler_length;i++)
  {
    if(handler_list[i].event==_EVENT_NULL||handler_list[i].event==ev.event){
      _Context *next=handler_list[i].handler(ev,context);
   // printf("event: %d length %d\n",ev.event,_handler_length);
      if(next)ret=next;
    }

  }
  if(ret==NULL){
    printf("event: %d length %d\n",ev.event,_handler_length);
    panic("os_trap");}
  return ret;
  //return context;
}

static void os_on_irq(int seq, int event, handler_t handler) {
    kmt->spin_lock(&lk_irq);
  //  printf("in irq\n");
   // printf("length:%d \n",_handler_length);
    if(_handler_length==0)
    {
      handler_list[_handler_length].seq=seq;
      handler_list[_handler_length].event=event;
      handler_list[_handler_length].handler=handler;

      _handler_length++;
      kmt->spin_unlock(&lk_irq);
      return;
    }
    else{
      int mid=0;
      int tmp=0;
      for(tmp=0;tmp<_handler_length;tmp++)
      {
        if(handler_list[tmp].seq<=seq)
        {
          continue;
        }else break;
      }        
      mid=tmp;
      //printf("mid %d\n",mid);
      _handler_length++;

      for(int i=_handler_length-1;i>mid;i--)
      {
        handler_list[i].seq=handler_list[i-1].seq;
        handler_list[i].event=handler_list[i-1].event;
        handler_list[i].handler=handler_list[i-1].handler;
        
      }

      // for(int i=0;i<_handler_length;i++)
      // {
      //   printf("%d： seq:%d event:%d \n",i,handler_list[i].seq,handler_list[i].event);
      // }

      
      handler_list[mid].seq=seq;
      handler_list[mid].event=event;
      handler_list[mid].handler=handler;
  

      if(mid-1>0)assert(handler_list[mid].seq>=handler_list[mid-1].seq);
      if(mid+1<_handler_length) assert(handler_list[mid].seq<=handler_list[mid+1].seq);
     // printf("before out of on_irq\n");
      //assert(handler_list[_handler_length-1].seq!=0x3f3f3f3f&&handler_list[_handler_length-1].event!=-123);
      kmt->spin_unlock(&lk_irq);
      return;
    }

}

MODULE_DEF(os) {
  .init   = os_init,
  .run    = os_run,
  .trap   = os_trap,
  .on_irq = os_on_irq,
};
