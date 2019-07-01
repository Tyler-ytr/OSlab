#include <vfs.h>
#include <blkfs.h>
#define vidx (&vinodes[index])
#define vnidx (&vinodes[next_index])
#define voidx (&vinodes[origin_index])
extern device_t *dev_lookup(const char*name);

struct filesystem filesystems[MAX_FS_NUM];
file_t flides[MAX_FILE_NUM];
vinode_t vinodes[MAX_VINODE_NUM];
//辅助函数;
void double_link_add(int origin_index,int next_index);
void double_link_add(int origin_index,int next_index);
//vfs inode table 操作;

static int vit_item_alloc();
static void vit_item_free(int index);
static int vit_lookup_cur(char *path,int* check,int dir_index);//从cur这个目录开始遍历inode_list链表;
static int lookup_root(char *path, int *find_flag, int *path_offset) ;

static int vit_item_alloc(){
  for (int i=0;i<MAX_VINODE_NUM;i++){
    if(vinodes[i].mode==UNUSED){
      vinodes[i].mode|=ALLOCED;
      return i;
    }
  }
  printf("Error： The vfs inode table is full!");
  return -1;
}
static void vit_item_free(int index){
  vinodes[index].mode=UNUSED;//不存在;
}
static int first_name_len(char *name){//返回/前面的第一个名字;
  int name_len=strlen(name); 
  int result=0;
  while(result<name_len&&(name[result]!='/')){
    result+=1;
  }
  return result;
}
static int check_item_match(const char *name1,const char *name2,int len){//name1放vit里面的成员,name2放待比较的外部字符串;
  if(strncmp(name1,name2,len)){
    return 0;
  }

  int result=(name1[len]=='\0');
  return result;
}
static int lookup_cur(char *path,int *find_flag,
                       int cur_inode,int *path_offset){
  //对于目录项cur_inode,遍历它的子节点,将name偏移量存储到path_offset里面,如果没找到find_flag=0;返回找到的inode;
  if(!strlen(path)){
    *find_flag=1;
    return cur_inode;
  }

  int k=0;
  int len=first_name_len(path);

  for(k=vinodes[cur_inode].child;k!=-1;k=vinodes[k].next){
    if(check_item_match(vinodes[k].name,path,len)){
      break;
    }
  }

  if(k==-1){
    *find_flag=0;//没有找到;
    printf("lookup_cur: File not found;\n");
    return cur_inode;
  }

 // 用于软链接； 
  int real_file=k;
  while(vinodes[real_file].mode&TYPE_LINK)
    real_file=vinodes[real_file].next_link;
//更新路径;
  char *newpath=path+(len+(path[len]=='/'?1:0));
  *path_offset+=len+(path[len]=='/'?1:0);
  return lookup_cur(newpath,find_flag,real_file,path_offset);
}
static int lookup_root(char *path, int *find_flag, int *path_offset) {
  return lookup_cur(path + 1, find_flag, VFS_ROOT, path_offset);
}
int get_name(char * path,int *name_len){
  int len=strlen(path);
  if(path[len-1]=='/'){
    path[len-1]='\0';
  }
  int cnt=0;

  for(int i=len;i>=0;i--){
    cnt++;
    if(path[i]=='/'){
      *name_len=cnt-1;//name_len是后面名字的长度;
      return i;}//返回的是/dev/able的最后一个/的位置;
  }
  return 0;
}

static int vinode_lookup(char *path){
  //根据path从 blkfs里面获得而信息;并且更新vfs的inode;


int len=strlen(path);
vinode_t buf;//inode 缓冲区;
int flag=0,offset=1;
int index=0;
if(path[len-1]=='/'){
  path[len-1]='\0';
}
if(path[0]=='/'){
 index=lookup_root(path,&flag,&offset);
}else{
  index=lookup_cur(path,&flag,VFS_ROOT,&offset);
}
if(flag==1){
  return index;
}
//int find_flag;
int file_len=first_name_len(path+offset);
//根据上面的两个lookup,可以确定这个index不在vinode_table里面,也就是说当前的目录应该是没有加载过得;
if(vinodes[index].child!=-1){return -1;}//非空矛盾错误;} 

if(vinodes[index].fs==NULL){
  return -1;//找不到系统的错误;
}
//现在开始,index是能找到的最后一个目录的inode,从blkfs里面找;
int result=-1;
int next_index=0;
int origin_index=-1;
int dir_index=-1,father_dir=-1;

int next_inode=-1;
for(int kth=0;kth<DIR_AMUT;kth++){
  result=vinodes[index].fs->readdir(vidx->fs,vidx->rinode_index,kth,&buf);
  if(result==0)continue;
  if((next_index=vit_item_alloc())==-1){
    printf("lookup；Vit is full. Wrong to vit_item_alloc,\n");
    return -1;
  }//新建一个vinode 保存在next_index里面;

  if(strcmp(buf.name,".")==0){
    if(origin_index!=-1){
      assert(0);//理论上不能修改;
    }

    vidx->child=next_index;
    //更新next_index的inode;
    strcpy(vnidx->path,vidx->path);
    strcpy(vnidx->name,".");
    
    vnidx->dir=-1;
    vnidx->father_dir=-1;//假文件;存疑;
    
    vnidx->next=-1;
    vnidx->child=index;

    vidx->pre_link=vidx->next_link=next_index;
    vnidx->refcnt=1;

    vnidx->mode=TYPE_LINK;//假文件,抽象成软链接;
    double_link_add(index,next_index);

    dir_index=next_index;//记录目录inode；
  }else if(strcmp(buf.name,"..")==0){
    if(voidx->next!=-1){
      assert(0);
    }//目前是链表的胃节点;

    voidx->next=next_index;
    voidx->father_dir=next_index;//".."是父亲目录的软链接;

    strcpy(vnidx->name,"..");
    strcpy(vnidx->path,vinodes[vinodes[vidx->dir].child].path);//连接到父亲dir的.这个假文件;

    vnidx->dir=origin_index;//在之前的文件目录里面;
    vnidx->father_dir=-1;

    vnidx->next=-1;
    vnidx->child=vinodes[vidx->father_dir].child;
    vnidx->pre_link=vnidx->next_link=next_index;
    vnidx->refcnt=1;

    vnidx->mode=TYPE_LINK;
    double_link_add(vinodes[vidx->father_dir].child,next_index);

    father_dir=next_index;
  }else{
    assert(dir_index!=-1&&father_dir!=-1);
    assert(voidx->next==-1);

    voidx->next=next_index;

    strcpy(vnidx->name,buf.name);
    strcpy(vnidx->path,vidx->path);
    strcat(vnidx->path,buf.name);

    if(buf.mode&TYPE_DIR){
      strcat(vnidx->path,"/");
    }
    printf("vnidx->path: %s\n",vnidx->path);

    vnidx->dir=dir_index;
    vnidx->father_dir=father_dir;

    vnidx->next=-1;
    vnidx->child=-1;

    vnidx->pre_link=vnidx->next_link=next_index;
    vnidx->refcnt=1;

    vnidx->mode=buf.mode;
  }

  vnidx->rinode_index=buf.rinode_index;
  vnidx->fs_type=vidx->fs_type;
  vnidx->fs=vidx->fs;
  origin_index=next_index;


  if(check_item_match(buf.name,path+offset,file_len)){
    assert(next_index==-1);
    next_inode=next_index;
    
  }

}
if(next_inode==-1){
  printf("Error: Directory exists, but can't find fild!\n");
  return -1;
}

  int noffset=1;
  index=(path[0] == '/') ? lookup_root(path, &flag, &noffset)
                         : lookup_cur(path, &flag, VFS_ROOT, &noffset);
  assert(noffset>offset);
  return (noffset == offset) ? -1 : vinode_lookup(path);

}


//　文件系统的操作;
static int filesystem_alloc(){
  for(int i=0;i<MAX_FS_NUM;i++){
    if(strlen(filesystems[i].name)==0){
      return i;
    }
  }
  return -1;//没有多余的了;
}
static int filesystem_free(int index){
  strcpy(filesystems[index].name,"");//清空名字;
  return 0;
}




char path_buf[MAX_PATH_LENGTH];

  void vfs_init(){
    return;
  };
  int vfs_access(const char *path, int mode){
    strcpy(path_buf,path);

    int index=vinode_lookup(path_buf);
    if(index==-1){
      return 1;
    }

    return (vinodes[index].mode&mode)!=mode;
  };
  int vfs_mount(const char *path, filesystem_t *fs){
    return 0;
  };
  int vfs_unmount(const char *path){
    return 0;
  };
  int vfs_mkdir(const char *path){
    return 0;
  };
  int vfs_rmdir(const char *path){
    return 0;
  };
  int vfs_link(const char *oldpath, const char *newpath){
    return 0;
  };
  int vfs_unlink(const char *path){
    return 0;
  };
  int vfs_open(const char *path, int flags){
    return 0;
  };
  ssize_t vfs_read(int fd, void *buf, size_t nbyte){
    return 0;
  }
  ssize_t vfs_write(int fd, void *buf, size_t nbyte){
    return 0;
  }
  off_t vfs_lseek(int fd, off_t offset, int whence){
    return 0;
  }
  int vfs_close(int fd){
    return 0;
  }
  int vfs_remove(const char *path){
    return 0;
  }
  int vfs_create(const char *path){
    return 0;
  }

  void vfs_ls(char * dir,char *buf){
    int index=vinode_lookup(dir);

    if(index==-1)return ;
    int offset=sprintf(buf,"\n");
    
    offset+=sprintf(buf+offset,"");

  printf("       index       name                  path\n");
  printf("cur:   %4d        %12s          %s\n\n", index, vinodes[index].name,
         vinodes[index].path);
  for (int k = vinodes[idx].child; k != -1; k = vinodes[k].next) {
    printf("child: %4d        %12s          %s\n", k, vinodes[k].name,
           vinodes[k].path);
  }
}

//辅助函数;
void double_link_add(int origin_index,int next_index){
  //双向链表 前一个节点是origin_index,后一个节点是next_index;

  int temp_index=vinodes[origin_index].next_link;
  vinodes[next_index].next_link=temp_index;
  vinodes[origin_index].next_link=next_index;
  vinodes[next_index].pre_link=origin_index;
  vinodes[temp_index].pre_link=next_index;

}
void double_link_remove(int index){
  int pre_link=vidx->pre_link;
  int next_link=vidx->next_link;

  vinodes[pre_link].next_link=vidx->next_link;
  vinodes[next_link].pre_link=vidx->pre_link;
}




MODULE_DEF(vfs){
    .init = vfs_init,
    .access = vfs_access,
    .mount = vfs_mount,
    .unmount = vfs_unmount,
    .create = vfs_create,
    .remove = vfs_remove,
    .link = vfs_link,
    .unlink = vfs_unlink,
    .open = vfs_open,
    .read = vfs_read,
    .write = vfs_write,
    .lseek = vfs_lseek,
    .close = vfs_close,
};

