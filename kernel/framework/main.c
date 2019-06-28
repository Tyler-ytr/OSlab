#include <kernel.h>
#include <devices.h>
#include <klib.h>
//#include <blkfs.h>//temp;
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
static void ls_function(device_t *tty,char *argv,char* pwd){
  printf("In ls");
  return ;
}
static void help_function(device_t *tty,char *argv,char*pwd){
  printf("In help");
  return;
}
static void error_function(device_t *tty,const char *argv){
  int offset=0;
  char text[256];
  
  offset+=sprintf(text+offset,"command not found: %s\n",argv);
  tty->ops->write(tty,0,text,strlen(text));

  return;
}

struct shell_function{
  char *function_name;
  void (*func)(device_t *tty,char *argv,char* pwd);
  int offset;
}Function[]={
  {"help ",help_function,5},
  {"ls ",ls_function,3}
};

static void shell_task(void *arg){
  char *name=(char*)arg;
  int function_num=sizeof(Function)/sizeof(struct shell_function);
  int find_func;
  char pwd[256];
  pwd[0]='/';
  pwd[1]='\0';
  //printf("%d\n\n\n\n\n\n\n\n",(int)_cpu());
  char text[128]="",readbuf[128]="",origin[128];
  device_t *tty=dev_lookup(name);
  while(1){
    sprintf(text,"(%s)$",name);
    tty->ops->write(tty,0,text,strlen(text));
    int nread=tty->ops->read(tty,0,readbuf,sizeof(readbuf));
    readbuf[nread-1]='\0';
    strcpy(origin,readbuf);
    
    printf("read: %s\n",readbuf);
    if(strcmp(readbuf,"ls")==0){
      strcpy(readbuf,"ls .");
    }
    if(strcmp(readbuf,"help")==0){
      strcpy(readbuf,"help ");
    }
    find_func=0;
    for(int i=0;i<function_num;i++){
      if(strncmp(readbuf,Function[i].function_name,Function[i].offset)==0)
      {
        (*Function[i].func)(tty,readbuf+Function[i].offset,pwd);
        find_func=1;break; 
      }
    } 
    if(find_func!=1){
      error_function(tty,origin);
    }
    


    
    //sprintf(text,"Echo:%s.\n",readbuf);
    
    tty->ops->write(tty,0,text,strlen(text));

  }
}
