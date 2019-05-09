#include <common.h>
#include <klib.h>
spinlock os_lk;

void test_from_yzy(){
      void *space[500];
          int i;
          for(int j=0;i<=7;j++){
          for(i=0;i<500;++i){
                    space[i]=pmm->alloc(rand()%((1<<10)-1)+4028);
                    printf("i: %d space:0x%x\n",i,space[i]);

          }}
          for(i=0;i<5000;++i){
                    int temp=rand()%10;
                            pmm->free(space[temp]);
                                    space[temp]=pmm->alloc(rand()&((1<<10)-1));
                    printf("temp: %d space:0x%x\n",temp,space[temp]);

          }
          for(i=0;i<500;++i){
                    pmm->free(space[i]);

          }
          printf("success!!!!!!");

}

static void os_init() {
  pmm->init();
  //To be continued:
  //kmt->init();
  //_vme_init(pmm->alloc, pmm->free);
  //dev->init();
  // 创建你的线程，线程可以调用`tty->ops->read`或`tty->ops->write`/
}
/*
static void hello() {
  for (const char *ptr = "Hello from CPU #"; *ptr; ptr++) {
    _putc(*ptr);
  }
  _putc("12345678"[_cpu()]); _putc('\n');
}
*/
static void os_run() {
//spinlock *lk=NULL;
 //spinlock * lk=&os_lk;
  //initlock(lk,NULL);
  //lock(lk);
  //int locked=0;
  //my_spin_lock(locked);
  //hello();
  test_from_yzy();
  //void *p=NULL;

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
  return context;
}

static void os_on_irq(int seq, int event, handler_t handler) {
}

MODULE_DEF(os) {
  .init   = os_init,
  .run    = os_run,
  .trap   = os_trap,
  .on_irq = os_on_irq,
};
