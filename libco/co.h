#ifndef __CO_H__
#define __CO_H__

typedef void (*func_t)(void *arg);
struct co;

void co_init();
struct co* co_start(const char *name, func_t func, void *arg);
void co_yield();
void co_wait(struct co *thd);

#define MAX_CO 200
#define STACK_SIZE 4096
enum STATUS {DEAD=0,READY=1,RUNNING=2,SUSPEND=3};//运行结束,开始运行,正在运行,挂起;
#endif
