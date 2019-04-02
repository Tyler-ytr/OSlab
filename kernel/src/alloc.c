#include <common.h>
#include <klib.h>

static uintptr_t pm_start, pm_end,start;

static void pmm_init() {
  pm_start = (uintptr_t)_heap.start;
  printf("start:0x%x",pm_start);
  pm_end   = (uintptr_t)_heap.end;
  start=pm_start;
}

static void *kalloc(size_t size) {
 pthread_t alloc_lock=0; 
 my_spin_lock(alloc_lock);
 printf("start1: 0x%x",start);
 start+=size;
 printf("start2: 0x%x",start);
 void *ret=&start; 
 printf(" ret:0x%x 0x%x\n",ret,*(int *)ret);
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
