#include <klib.h>
#include <procfs.h>
#include <vfs.h>
#include <common.h>

proc_t proc_list[MAX_PROC_NUM];
char *proc_names[]={".","..","cpuinfo","meminfo"};
int proc_total=4;
uint64_t total_memory=0;
uint64_t used_memory=0;
int cpu_num;
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
void procfs_init(filesystem_t *fs,const char *name,device_t *dev){
cpu_num=_ncpu();
  int temp_num=proc_total;
  proc_total=0;
  const char* name0=proc_names[0];
  procfs_add(name0);
  const char* name1=proc_names[1];
  procfs_add(name1);
  const char* name2=proc_names[2];
  procfs_add(name2);
  const char* name3=proc_names[3];
  procfs_add(name3);
  proc_total=temp_num;
  return ;
}
int procfs_readdir(filesystem_t *fs, int ridx, int kth, vinode_t *buf){
  int i=0;
  int cnt=0;
    int temp_inode=i-4; 
  for(i=0;i<proc_total;i++){
    temp_inode=i-4; 
    if(++cnt==kth){
      buf->rinode_index=i;
      switch (i)
      {
      case 0:
        strcpy(buf->name,proc_list[i].name);
        buf->mode=TYPE_DIR;
        break;
      case 1:
        strcpy(buf->name,proc_list[i].name);
        buf->mode=TYPE_DIR;
        break;
      case 2:
        strcpy(buf->name,proc_list[i].name);
        buf->mode=TYPE_FILE|RD_ABLE;
        break;
      case 3:
        strcpy(buf->name,proc_list[i].name);
        buf->mode=TYPE_FILE|RD_ABLE;
        break;
      default:
        sprintf(buf->name,"%d",temp_inode);
        buf->mode=TYPE_FILE|RD_ABLE;
        break;
      }
      return 1;
    }

  }


  return 0;
}
void procfs_info(){
  for(int i=0;i<proc_total;i++){
    printf("proc name: %s, shedule_time :%d cpu_num:%d \n",proc_list[i].name,proc_list[i].schedule_time,proc_list[i].cpu_num);
  }
//int temp=used_memory_info();
  printf("Total memory: %d \n",total_memory);
  printf("using memory: %d \n",used_memory);
return;
}

ssize_t procfs_read(int index, uint64_t offset, char* buf){

int result=0;
switch (index)
{
case 2:
  result+=sprintf(buf+result,"-----cpu info-----\n");
  result+=sprintf(buf+result,"Cpu num: %d\n",cpu_num);
  break;
case 3:
  result+=sprintf(buf+result,"-----memory info-----\n");
  int can_be_used=total_memory-used_memory;
  int tot=total_memory/1024;
  int can=can_be_used/1024;
  int used=used_memory/1024;
  result+=sprintf(buf+result,"Total memory: %d\n",tot);
  result+=sprintf(buf+result,"Used memory: %d\n",used);
  result+=sprintf(buf+result,"Left memory: %d\n",can);
  break;


default:
  
  result+=sprintf(buf+result,"-----process info-----\n");
  result+=sprintf(buf+result,"pid: %d\n",index-4);
  result+=sprintf(buf+result,"name: %s\n",proc_list[index].name);
  result+=sprintf(buf+result,"cpu num: %d\n",proc_list[index].cpu_num);
  result+=sprintf(buf+result,"schedule times: %d\n",proc_list[index].schedule_time);
  break;
}
return result;


                  }












