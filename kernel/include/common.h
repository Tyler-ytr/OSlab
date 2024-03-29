#ifndef __COMMON_H__
#define __COMMON_H__

#include <kernel.h>
#include <nanos.h>
#include <klib.h>
#include "vfs.h"
#include "debug.h"
#include <procfs.h>

//<<<<<<< HEAD
extern intptr_t my_spin_lock(intptr_t locked);
extern intptr_t my_spin_unlock(intptr_t locked);

#define pthread_t intptr_t

typedef struct _LIST_for_alloc
{
  struct _LIST_for_alloc *next;
  struct _LIST_for_alloc *prev;
  void * addr;
  int flag;//0,1 用来标示这块用了没,2表示这是head,3表示是unused_space;
  int size;// 单位是byte
}_node,*_list;
/*
typedef struct BOUND{
 struct BOUND* prev;
 struct BOUND* next;
 void *left_bound;//记录bound块的右边缘,_node块的左边缘,是_node块的起始位置;
 void *right_bound;//记录目前_node块的右边缘,是实际数据区的起始位置
 void *using_one;//记录目前正在使用的_node 的右边界,using_one应该是下个_node的起始位置;
}bound;// 我准备在堆区动态的建立存放链表,bound用来指这块地方的边界以及正在使用的链表的位置；
*///废弃这个设计,
//|head|size|pnode1|size|.....
_list cpu_head[9];
_list unused_space;
_list head;
void* max;
//_node test;

/*
typedef struct Spinlock{
  char *name;
  int locked;
  int cpu;
}spinlock;
*/

typedef unsigned int   uint;


typedef struct Spinlock {
  uint locked;       // Is the lock held?

  // For debugging:
  char *name;        // Name of lock.
  int cpu;   // The cpu holding the lock.
  uint pcs[10];      // The call stack (an array of program counters)
                     // that locked the lock.
}spinlock;




extern void unlock(struct Spinlock *lk);
extern void initlock(struct Spinlock *lk,char *name);
extern void lock(struct Spinlock *lk);


static inline unsigned int
readeflags(void)
{
  uint eflags;
  asm volatile("pushfl; popl %0" : "=r" (eflags));
  return eflags;
}

/*
struct cpu {
  unsigned char  apicid;                // Local APIC ID
  struct context *scheduler;   // swtch() here to enter scheduler
  //struct taskstate ts;         // Used by x86 to find stack for interrupt
//  struct segdesc gdt[NSEGS];   // x86 global descriptor table
  volatile uint started;       // Has the CPU started?
  int ncli;                    // Depth of pushcli nesting.
  int intena;                  // Were interrupts enabled before pushcli?
  struct proc *proc;           // The process running on this cpu or null
};*/

//int intena[8];
#define KERNBASE 0x80000000
//void            panic(char*) __attribute__((noreturn));
//struct cpu*     mycpu(void);


//#define panic my_printf

//一堆spin_lock
//spinlock init_lk;
//spinlock os_lk;
//=======
/*
struct task {
  const char *name;
  _Context context;
  char fence_1[32]={
                  (char)0xcc,(char)0xcc,(char)0xcc,(char)0xcc,
                  (char)0xcc,(char)0xcc,(char)0xcc,(char)0xcc,
                  (char)0xcc,(char)0xcc,(char)0xcc,(char)0xcc,
                  (char)0xcc,(char)0xcc,(char)0xcc,(char)0xcc,
                  (char)0xcc,(char)0xcc,(char)0xcc,(char)0xcc,
                  (char)0xcc,(char)0xcc,(char)0xcc,(char)0xcc,
                  (char)0xcc,(char)0xcc,(char)0xcc,(char)0xcc,
                  (char)0xcc,(char)0xcc,(char)0xcc,(char)0xcc};
  char stack[4096];
  char fence_2[32]={
                  (char)0xcc,(char)0xcc,(char)0xcc,(char)0xcc,
                  (char)0xcc,(char)0xcc,(char)0xcc,(char)0xcc,
                  (char)0xcc,(char)0xcc,(char)0xcc,(char)0xcc,
                  (char)0xcc,(char)0xcc,(char)0xcc,(char)0xcc,
                  (char)0xcc,(char)0xcc,(char)0xcc,(char)0xcc,
                  (char)0xcc,(char)0xcc,(char)0xcc,(char)0xcc,
                  (char)0xcc,(char)0xcc,(char)0xcc,(char)0xcc,
                  (char)0xcc,(char)0xcc,(char)0xcc,(char)0xcc};
};*/
typedef struct file file_t;
struct task{
  volatile int status;//0 没有初始化 1 runningable 2 running 3 waiting 
  int alive;
  const char *name;
  _Context context;
  _Area stack;// in am.h void* start,void* end;
  sem_t *waiting_sem;//记录信号量
  //struct task *prev;
  struct task *next;
  void*proc;
  //file_t *fildes[32];//最多开32个文件;
};

struct spinlock {
  uint locked;       // Is the lock held?

  // For debugging:
  const char *name;        // Name of lock.
  int cpu;   // The cpu holding the lock.
  //uint pcs[10];      // The call stack (an array of program counters)
                     // that locked the lock.
};
struct semaphore {
  spinlock_t lock;
  int value;
  const char * name;
  task_t *task_list[256];
  int start;
  int end; 
  int MAXSIZE;//=256;
};

typedef struct HANDLER_LIST{
  int seq/*=0x3f3f3f3f*/;
  int event/*=-123*/;
  handler_t handler;
}Handler_list;
#define MAX_HANDLIST_NUMBER 1024
#define MAX_STACK_SIZE 4096*2
static inline void panic(const char *s) { printf("%s\n", s); _halt(1); }
//>>>>>>> dad0034cd442046d9cc407654dce68cdc0fd783e

// //下面是关于 vfs的内容:
// #define MAX_FS_NAME_LEN 128
// #define MAX_INODE_NAME_LEN 128
// #define BLOCKSIZE 1024
// typedef struct filesystem filesystem_t;
// typedef struct fsops fsops_t;
// typedef struct file file_t;
// typedef struct inode inode_t;



// struct filesystem{
//   char name[MAX_FS_NAME_LEN];



// };






#endif
