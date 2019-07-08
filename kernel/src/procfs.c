#include <klib.h>
#include <procfs.h>
#include <vfs.h>

proc_t proc_list[MAX_PROC_NUM];

int proc_total=4;
uint64_t total_memory=0;
uint64_t using_memory=0;
//extern uint64_t used_memory_info();




void * procfs_add(const char *name){
  int index;
  if(proc_total>=MAX_PROC_NUM-1){
    printf("The proc_list is full!");
    return NULL;
  }
  index=proc_total;
  proc_total+=1;
  proc_list[index].name=name;
  proc_list[index].cpu_num=_cpu();
  proc_list[index].schedule_time=0;
//  proc_list[index].mem_size=0;
  return &proc_list[index];
}

int procfs_total(){
  return proc_total;
}


void procfs_info(){
  for(int i=0;i<proc_total;i++){
    printf("proc name: %s, shedule_time :%d cpu_num:%d \n",proc_list[i].name,proc_list[i].schedule_time,proc_list[i].cpu_num);
  }
//int temp=used_memory_info();
  printf("Total memory: %d used_memory:%d \n",total_memory,using_memory);
return;
}













