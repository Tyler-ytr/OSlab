#include <kernel.h>
#include <devices.h>
#include <klib.h>
static void echo_task(void *arg);
int main() {
  _ioe_init();
  _cte_init(os->trap);

  // call sequential init code
  os->init();
//kmt->create(pmm->alloc(sizeof(task_t)), "print", echo_task, "tty1");
//kmt->create(pmm->alloc(sizeof(task_t)), "print", echo_task, "tty2");
//kmt->create(pmm->alloc(sizeof(task_t)), "print", echo_task, "tty3");
//kmt->create(pmm->alloc(sizeof(task_t)), "print", echo_task, "tty4");
  _mpe_init(os->run); // all cores call os->run()

  return 1;
}

static void echo_task(void *arg){
  char *name=(char*)arg;
  char text[128]="",line[128]="";
  device_t *tty=dev_lookup(name);
  while(1){
    sprintf(text,"(%s)$",name);
    tty->ops->write(tty,0,text,strlen(text));
    int nread=tty->ops->read(tty,0,line,sizeof(line));
    line[nread-1]='\0';
    sprintf(text,"Echo:%s.\n",line);
    tty->ops->write(tty,0,text,strlen(text));
  }
}