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

  sprintf(text, "echo:%s\n", argv);
  tty->ops->write(tty,0,text,strlen(text));
}
extern char *vfs_real_path(const char *path);
static void cd_function(device_t *tty,char*argv,char *pwd){

  sprintf(text, "cd:%s\n", argv);
  change_into_abs_path(argv,pwd);
  int find_flag=vfs_access(abs_path,TYPE_DIR);
  if(find_flag==1){//没找到
    sprintf(text,"Dir %s cannot be found!\n",argv);
 //   tty->ops->write(tty,0,text,strlen(text));
  //  return ;
  }else{
   printf("in else\n");
    int abs_length=strlen(abs_path);
    if(abs_path[abs_length-1]=='/'){
      strcat(abs_path,".");//软链接位置;
    }
    else{
      strcat(abs_path,"/.");
    } 
    
    printf("abs_path: %s\n",abs_path);
    find_flag=vfs_access(abs_path,TYPE_DIR);
    if(find_flag==0){
      strcpy(pwd,vfs_real_path(abs_path));
    }

    sprintf(text,"Current dir:%s\n",pwd);
  }

  tty->ops->write(tty,0,text,strlen(text));
}
static void pwd_function(device_t *tty,char*argv,char *pwd){
  //int offset=0;
  sprintf(text,"%s\n",pwd);
  tty->ops->write(tty,0,text,strlen(text));
}
extern void vfs_info();
static void info_function(device_t *tty,char*argv,char *pwd){
  //int offset=0;
 // sprintf(text,"%s\n",pwd);
  vfs_info();
  //tty->ops->write(tty,0,text,strlen(text));
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
static void touch_function(device_t *tty,char *argv,char * pwd){
 change_into_abs_path(argv,pwd);
  //找同目录同名文件;
  int result=vfs_access(abs_path,TYPE_DIR);
  if(result==0){
     sprintf(text,"File %s already exists!!\n",argv);
  }else{
       int flag=vfs_create_file(abs_path);
    switch(flag){
      case 0:
             sprintf(text,"File %s successfully create!\n",argv);
             break;
      case -1:
             sprintf(text,"The name of file %s is not accepted\n",argv);
             break;
      case -2:
             sprintf(text,"The filesystem cannot touch file!Only ext2fs can mkdir!\n",argv);
             break;
      default:
             sprintf(text,"Undefined behaviour!\n",argv);
             break;
  }

}
   tty->ops->write(tty,0,text,strlen(text));

}

static void mkdir_function(device_t *tty,char *argv,char * pwd){
  change_into_abs_path(argv,pwd);
  //找同目录同名;
  int result=vfs_access(abs_path,TYPE_DIR);
  if(result==0){
    sprintf(text,"Dir %s already exists!!\n",argv);
  }else{
    int flag=vfs_mkdir(abs_path);
    switch(flag){
      case 0:
             sprintf(text,"Dir %s successfully create!\n",argv);
             break;
      case -1:
             sprintf(text,"The name of dir %s is not accepted\n",argv);
             break;
      case -2:
             sprintf(text,"The filesystem cannot mkdir!Only ext2fs can mkdir!\n",argv);
             break;
      default:
             sprintf(text,"Undefined behaviour!\n",argv);
             break;
    }
  }
  tty->ops->write(tty,0,text,strlen(text));

}
static void rmdir_function(device_t *tty,char *argv,char * pwd){
  change_into_abs_path(argv,pwd);
  int result=vfs_access(abs_path,TYPE_DIR);
  if(result==1){
    sprintf(text,"Dir %s doesn't exist!!\n",argv);
  }else{
    int flag=vfs_rmdir(abs_path);
    switch(flag){
      case 0:
             //sprintf(text,"Dir %s successfully remove!\n",argv);
             sprintf(text,"Successfully rmdir %s!\n",argv);
             break;
      case -1:
             sprintf(text,"The name of dir %s is incorrect\n",argv);
             break;
      case -2:
             sprintf(text,"Ext2fs cannot rmdir %s.\n",argv);
             break;
      case -3:
             sprintf(text,"The filesystem cannot rmdir! Only ext2fs can rmdir!\n",argv);
             break;
      default:
             sprintf(text,"Undefined behaviour!\n",argv);
             break;
    }
  }
  tty->ops->write(tty,0,text,strlen(text));

}

static void rm_function(device_t *tty,char *argv,char * pwd){//删除文件;
 change_into_abs_path(argv,pwd);
 //printf("abs_path:%s",abs_path);
  int result=vfs_access(abs_path,TYPE_FILE);
  if(result==1){
    sprintf(text,"File %s doesn't exist!!\n",argv);
  }else{
    int flag=vfs_remove_file(abs_path);
    switch(flag){
      case 0:
             //sprintf(text,"Dir %s successfully remove!\n",argv);
             sprintf(text,"Successfully remove %s!\n",argv);
             break;
      case -1:
             sprintf(text,"The name of file %s is incorrect\n",argv);
             break;
      case -2:
             sprintf(text,"Ext2fs cannot remove %s.\n",argv);
             break;
      case -3:
             sprintf(text,"The filesystem cannot remove file! Only ext2fs can remove file!\n",argv);
             break;
      default:
             sprintf(text,"Undefined behaviour!\n",argv);
             break;
    }
  }
  tty->ops->write(tty,0,text,strlen(text));change_into_abs_path(argv,pwd);
 //int result=vfs_access(abs_path,TYPE_FILE);


return ;
}
static void cat_function(device_t *tty,char *argv,char * pwd){
  //调用read;
 change_into_abs_path(argv,pwd);
 int fd=vfs_open(abs_path,O_RDONLY);
 int length=strlen(abs_path);
 //printf("abs: %s,%d",abs_path,length);
 int offset1=0;
 for(int i=length;i>=0;i--){
   if(abs_path[i]=='/'){
     offset1=i;break;
   }
 }
 //printf("abs: %c,%c",abs_path[offset1],abs_path[offset1-1]);
int proc_flag=0;
if(offset1>=4){
  if(
    abs_path[offset1-1]=='c'&&
    abs_path[offset1-2]=='o'&&
    abs_path[offset1-3]=='r'&&
    abs_path[offset1-4]=='p'
  )
  proc_flag=1;
}
 //printf("fd:%d\n",fd);
 if(fd==-1){
   //printf("GG!\n");
   return ;
 }else if(proc_flag==0){
  while(vfs_read(fd,text,128))
  tty->ops->write(tty,0,text,strlen(text));
 }else if(proc_flag==1){
  vfs_read(fd,text,128);
  tty->ops->write(tty,0,text,strlen(text));

 }
  
  return;
}
//ssize_t vfs_write(int fd, void *buf, size_t nbyte)
static void write_function(device_t * tty,char * argv,char * pwd){
change_into_abs_path(argv,pwd);
printf("abs_path:%s",abs_path);

int fd=vfs_open(abs_path,O_RDWR);

if(fd==-1){
  sprintf(text,"There is no such file!\n");
  tty->ops->write(tty,0,text,strlen(text));
  return;
}

for(;;){
  int offset=tty->ops->read(tty,0,text,128);  
  printf("write text:%s",text);

  if(text[offset-2]=='*'){
    vfs_write(fd,text,offset-2);
    return;
  }else{
    vfs_write(fd,text,offset);
  }
}
return ;

}
extern int procfs_info();
static void temp_function(device_t * tty,char * argv,char * pwd){
procfs_info();
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
  {"echo ",echo_function,5},
  {"cd ",cd_function,3},
  {"info ",info_function,5},
  {"mkdir ",mkdir_function,6},
  {"rmdir ",rmdir_function,6},
  {"touch ",touch_function,6},
  {"cat ",cat_function,4},
  {"rm ",rm_function,3},
  {"write ",write_function,6},
  {"procinfo ",temp_function,9}
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
       if(strcmp(readbuf,"echo")==0){
         printf("here\n");
      strcpy(readbuf,"echo ");
    }
    if(strcmp(readbuf,"help")==0){
      strcpy(readbuf,"help ");
    }
      if(strcmp(readbuf,"pwd")==0){
      strcpy(readbuf,"pwd ");
    }
    //printf("readbuf:%s\n",readbuf);
    
    find_func=0;
    for(int i=0;i<function_num;i++){
      //int result=strncmp(readbuf,Function[i].function_name,Function[i].offset);
     // printf("%d\n",result);
      if(strncmp(readbuf,Function[i].function_name,Function[i].offset)==0)
      {
       // printf("argv:%s\n",readbuf+Function[i].offset);
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
