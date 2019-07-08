#include <klib.h>
#include <procfs.h>
#include <vfs.h>

proc_t proc_list[MAX_PROC_NUM];

int proc_total=4;
uint64_t total_memory=0;
uint64_t used_memory=0;





void * profs_add(const char *name){
  int index;
  if(proc_total>=MAX_PROC_NUM-1){
    printf("The proc_list is full!");
    return NULL;
  }
  index=proc_total;
  proc_total+=1;
  proc[index].name=name;
  proc[index].cpu_num=_cpu();
  proc[index].schedule_time=0;
  proc[index].mem_size=0;
  return &proc[index];
}













