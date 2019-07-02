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
  char text1[128]="",line[128]="";
  device_t *tty=dev_lookup(name);
  while(1){
    sprintf(text1,"(%s)$",name);
  //printf("%d\n\n\n\n\n\n\n\n",(int)_cpu());
    tty->ops->write(tty,0,text1,strlen(text1));
  //printf("%d\n\n\n\n\n\n\n\n",(int)_cpu());
    int nread=tty->ops->read(tty,0,line,sizeof(line));
 // printf("%d\n\n\n\n\n\n\n\n",(int)_cpu());
    line[nread-1]='\0';
    sprintf(text1,"Echo:%s.\n",line);
  //printf("%d\n\n\n\n\n\n\n\n",(int)_cpu());
    tty->ops->write(tty,0,text1,strlen(text1));
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
char text[2048]={};
char abs_path[256];//记得改成二维数组存tty;
static void change_into_abs_path(char *name,char*pwd){
  if(name[0]!='/'){
    strcpy(abs_path,pwd);
    strcat(abs_path,name);
  }else{
    strcat(abs_path,name);

  }
}
static void echo_function(device_t *tty,char*argv,char *pwd){

  sprintf(text, "%s\n", argv);
  tty->ops->write(tty,0,text,strlen(text));
}
static void pwd_function(device_t *tty,char*argv,char *pwd){
  //int offset=0;
  sprintf(text,"%s\n",pwd);
  tty->ops->write(tty,0,text,strlen(text));
}
extern void vfs_ls(char * dir,char *buf);
static void ls_function(device_t *tty,char *argv,char* pwd){
 // printf("In ls");
  change_into_abs_path(argv,pwd);
  //printf("In ls");
  int abs_path_length=strlen(abs_path);
  if(abs_path[abs_path_length-1]=='/'){
    strcat(abs_path,".");
  }else{
    strcat(abs_path,"/.");
  }
  //printf("In ls");
  vfs_ls(abs_path,text);
  //printf("In ls");
  //printf("%s\n",text);
  tty->ops->write(tty,0,text,strlen(text));

  return ;
}
static void help_function(device_t *tty,char *argv,char*pwd){
  printf("In help");
  return;
}
static void error_function(device_t *tty,const char *argv){
  int offset=0;
  
  offset+=sprintf(text+offset,"command not found: %s. Input \'help\' for more information.\n",argv);
  tty->ops->write(tty,0,text,strlen(text));

  return;
}

struct shell_function{
  char *function_name;
  void (*func)(device_t *tty,char *argv,char* pwd);
  int offset;
}Function[]={
  {"help ",help_function,5},
  {"ls ",ls_function,3},
  {"pwd ",pwd_function,4},
  {"echo ",echo_function,5}
};

static void shell_task(void *arg){
  char *name=(char*)arg;
  int function_num=sizeof(Function)/sizeof(struct shell_function);
  int find_func;
  char pwd[256];
  pwd[0]='/';
  pwd[1]='\0';
  //printf("%d\n\n\n\n\n\n\n\n",(int)_cpu());
  char readbuf[128]="",origin[128];
  device_t *tty=dev_lookup(name);
  while(1){
    sprintf(text,"(%s)$",name);
    tty->ops->write(tty,0,text,strlen(text));
    int nread=tty->ops->read(tty,0,readbuf,sizeof(readbuf));
    readbuf[nread-1]='\0';
    strcpy(origin,readbuf);
    
    if(strcmp(readbuf,"ls")==0){
      strcpy(readbuf,"ls ");
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
    


    
   // sprintf(text,"Echo:%s.\n",readbuf);
    printf("result: %s\n",readbuf);
    
   // tty->ops->write(tty,0,text,strlen(text));

  }
}
