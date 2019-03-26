#include <common.h>
#include <klib.h>

static uintptr_t pm_start, pm_end,start;

static void pmm_init() {
  pm_start = (uintptr_t)_heap.start;
  pm_end   = (uintptr_t)_heap.end;
  start=pm_start;
}

static void *kalloc(size_t size) {
 pthread_t *alloc_lock=NULL; 
 my_spin_lock(alloc_lock);
 start+=size;
 void *ret=&start; 
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
