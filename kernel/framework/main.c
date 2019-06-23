#include <kernel.h>
#include <devices.h>
#include <klib.h>
static void echo_task(void *arg);
static void echo_task2(void *arg);
static void shell_task(void *arg);
int main() {
  _ioe_init();
  _cte_init(os->trap);

  // call sequential init code
  os->init();
kmt->create(pmm->alloc(sizeof(task_t)), "print1", shell_task, "tty1");
kmt->create(pmm->alloc(sizeof(task_t)), "print2", echo_task, "tty2");
kmt->create(pmm->alloc(sizeof(task_t)), "print3", echo_task2, "tty3");
//kmt->create(pmm->alloc(sizeof(task_t)), "print4", echo_task, "tty3");
kmt->create(pmm->alloc(sizeof(task_t)), "print4", echo_task, "tty4");
  _mpe_init(os->run); // all cores call os->run()

  return 1;
}

static void echo_task(void *arg){
  char *name=(char*)arg;
 // printf("%d\n\n\n\n\n\n\n\n",(int)_cpu());
  char text[128]="",line[128]="";
  device_t *tty=dev_lookup(name);
  while(1){
    sprintf(text,"(%s)$",name);
  //printf("%d\n\n\n\n\n\n\n\n",(int)_cpu());
    tty->ops->write(tty,0,text,strlen(text));
  //printf("%d\n\n\n\n\n\n\n\n",(int)_cpu());
    int nread=tty->ops->read(tty,0,line,sizeof(line));
 // printf("%d\n\n\n\n\n\n\n\n",(int)_cpu());
    line[nread-1]='\0';
    sprintf(text,"Echo:%s.\n",line);
  //printf("%d\n\n\n\n\n\n\n\n",(int)_cpu());
    tty->ops->write(tty,0,text,strlen(text));
  }
}
static void echo_task2(void *arg){
  char *name=(char*)arg;
 // printf("%d\n\n\n\n\n\n\n\n",(int)_cpu());
  char text[128]="",line[128]="";
  device_t *tty=dev_lookup(name);
  while(1){
    sprintf(text,"(%s)$",name);
 // printf("%d\n\n\n\n\n\n\n\n",(int)_cpu());
    tty->ops->write(tty,0,text,strlen(text));
 // printf("%d\n\n\n\n\n\n\n\n",(int)_cpu());
    int nread=tty->ops->read(tty,0,line,sizeof(line));
 // printf("%d\n\n\n\n\n\n\n\n",(int)_cpu());
    line[nread-1]='\0';
    sprintf(text,"Echo:%s.\n",line);
  //printf("%d\n\n\n\n\n\n\n\n",(int)_cpu());
    tty->ops->write(tty,0,text,strlen(text));
  }
}


// void shell_thread(int tty_id) {
//   char buf[128];
//   sprintf(buf, "/dev/tty%d", tty_id);
//   //int stdin = vfs->open(buf, O_RDONLY);
//   //int stdout = vfs->open(buf, O_WRONLY);
//   while (1) {
//     if (got_a_line()) {
//        ...
//     } else {
//       ssize_t nread = fs->read(stdin, buf, sizeof(buf));
//       ...
//     }
//     // supported commands:
//     //   ls
//     //   cd /proc
//     //   cat filename
//     //   mkdir /bin
//     //   rm /bin/abc
//     //   ...
//   }
// }


static void shell_task(void *arg){
  char *name=(char*)arg;
  char pwd[256];
  //printf("%d\n\n\n\n\n\n\n\n",(int)_cpu());
  char text[128]="",line[128]="";
  device_t *tty=dev_lookup(name);
  while(1){
    sprintf(text,"(%s)$",name);
    tty->ops->write(tty,0,text,strlen(text));
    int nread=tty->ops->read(tty,0,line,sizeof(line));
    line[nread-1]='\0';
    
    printf("read: %s\n",line);


    
    
    
    
    sprintf(text,"Echo:%s.\n",line);
    tty->ops->write(tty,0,text,strlen(text));

  }
}
