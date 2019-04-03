#include <common.h>
#include <klib.h>
static inline spinlock os_lk;

void test_from_yzy(){
      void *space[100];
          int i;
          for(i=0;i<100;++i){
                    space[i]=pmm->alloc(rand()%((1<<10)-1));
                    printf("i: %d space:0x%x\n",i,*(int*)space[i]);
                        
          }
          for(i=0;i<1000;++i){
                    int temp=rand()%10;
                            pmm->free(space[temp]);
                                    space[temp]=pmm->alloc(rand()&((1<<10)-1));
                                        
          }
          for(i=0;i<100;++i){
                    pmm->free(space[i]);
                        
          }

}

static void os_init() {
  pmm->init();
}

static void hello() {
  for (const char *ptr = "Hello from CPU #"; *ptr; ptr++) {
    _putc(*ptr);
  }
  _putc("12345678"[_cpu()]); _putc('\n');
}

static void os_run() {
//spinlock *lk=NULL;
 spinlock * lk=&os_lk;
  initlock(lk,NULL);
  lock(lk);
  //int locked=0;
  //my_spin_lock(locked);
  hello();
  //test_from_yzy();
  //void *p=NULL;
  
 // p=pmm->alloc(1000);
  //printf("p:0x%x\n",p);
  //my_spin_unlock(locked);
  unlock(lk);
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
