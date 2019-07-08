#ifndef _PROCFS_H
#define _PROCFS_H

//#include <devices.h>

#define MAX_PROC_NUM 128
typedef struct proc proc_t;

struct proc{
  const char*name;
  int cpu_num; 
  int mem_size;
  int schedule_time;
};


void * profs_add(const char *name);


#endif