#include <vfs.h>
#include <blkfs.h>
#define vidx (&vinodes[index])
#define vnidx (&vinodes[next_index])
#define voidx (&vinodes[origin_index])
#define vdir (&vinodes[dir])
#define vfat (&vinodes[father_dir])
extern device_t *dev_lookup(const char*name);

struct filesystem filesystems[MAX_FS_NUM];
file_t flides[MAX_FILE_NUM];
vinode_t vinodes[MAX_VINODE_NUM];
//辅助函数;
void double_link_add(int origin_index,int next_index);
void double_link_remove(int index);
//vfs inode table 操作;

static int vit_item_alloc();
static void vit_item_free(int index);
static int lookup_cur(char *path,int *find_flag,
                       int cur_inode,int *path_offset);
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
static int first_name_len(const char *name){//返回/前面的第一个名字;
  // int name_len=strlen(name); 
  // int result=0;
  // while(result<name_len&&(name[result]!='/')){
  //   result+=1;
  // }
  // return result;
    int ret = 0;
  for (; name[ret] && name[ret] != '/';) ret++;
  return ret;
}
static int path_length_offset(const char *path){
  int total_length=strlen(path);
  for(int i=total_length;i>=0;i--){
    if(path[i]=='/')return i;
  }
  return 0;
}
static int check_item_match(const char *name1,const char *name2,int len){//name1放vit里面的成员,name2放待比较的外部字符串;
  if(strncmp(name1,name2,len)){
    return 0;
  }

 // int result=(name1[len]=='\0');
  return name1[len]=='\0';
}
static int lookup_cur(char *path,int *find_flag,
                       int cur_inode,int *path_offset){
  //对于目录项cur_inode,遍历它的子节点,将name偏移量存储到path_offset里面,如果没找到find_flag=0;返回找到的inode;
 // printf("In lookup_cur\n");
 // printf("path:%s\n",path);
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
    {real_file=vinodes[real_file].next_link;
    //printf("///sdsdsd\n");
    }
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
//   //根据path从 blkfs里面获得而信息;并且更新vfs的inode;

// printf("lookup here\n");

// int len=strlen(path);
// vinode_t buf;//inode 缓冲区;
// int flag=0,offset=1;
// if(path[len-1]=='/'){
//   path[len-1]='\0';
// }

// // if(path[0]=='/'){
// //  index=lookup_root(path,&flag,&offset);
// // }else{
// //   index=lookup_cur(path,&flag,VFS_ROOT,&offset);
// // }
// int index=(path[0] == '/') ? lookup_root(path, &flag, &offset)
//                              : lookup_cur(path, &flag, VFS_ROOT, &offset);
// if(flag==1){
//   return index;
// }
// //int find_flag;
// int file_len=first_name_len(path+offset);
// //根据上面的两个lookup,可以确定这个index不在vinode_table里面,也就是说当前的目录应该是没有加载过得;
// if(vinodes[index].child!=-1){return -1;}//非空矛盾错误;} 

// if(vinodes[index].fs==NULL){
//   return -1;//找不到系统的错误;
// }
// //现在开始,index是能找到的最后一个目录的inode,从blkfs里面找;
// int result=-1;
// int next_index=0;
// int origin_index=-1;
// int dir_index=-1,father_dir=-1;
// int kth=0;

// int next_inode=-1;
// //for(int kth=0;kth<DIR_AMUT;kth++){
//  // result=vinodes[index].fs->readdir(vidx->fs,vidx->rinode_index,kth,&buf);
//   while((result=vidx->fs->readdir(vidx->fs,vidx->rinode_index,++kth,&buf))){
//   printf("name:%s\n",buf.name);
//   if((next_index=vit_item_alloc())==-1){
//     printf("lookup；Vit is full. Wrong to vit_item_alloc,\n");
//     return -1;
//   }//新建一个vinode 保存在next_index里面;

//   if(strcmp(buf.name,".")==0){
//     printf("find .\n");
//     if(origin_index!=-1){
//       assert(0);//理论上不能修改;
//     }

//     vidx->child=next_index;
//     //更新next_index的inode;
//     strcpy(vnidx->name,".");
//     strcpy(vnidx->path,vidx->path);
    
//     vnidx->dir=-1;
//     vnidx->father_dir=-1;//假文件;存疑;
    
//     vnidx->next=-1;
//     vnidx->child=index;

//     vidx->pre_link=vidx->next_link=next_index;
//     vnidx->refcnt=1;

//     vnidx->mode=TYPE_LINK;//假文件,抽象成软链接;
//     double_link_add(index,next_index);

//     dir_index=next_index;//记录目录inode；
//   }else if(strcmp(buf.name,"..")==0){
//     printf("find ..\n");
//     if(voidx->next!=-1){
//       assert(0);
//     }//目前是链表的胃节点;

//     voidx->next=next_index;
//     voidx->father_dir=next_index;//".."是父亲目录的软链接;

//     strcpy(vnidx->name,"..");
//     strcpy(vnidx->path,vinodes[vinodes[vidx->dir].child].path);//连接到父亲dir的.这个假文件;

//     vnidx->dir=origin_index;//在之前的文件目录里面;
//     vnidx->father_dir=-1;

//     vnidx->next=-1;
//     vnidx->child=vinodes[vidx->father_dir].child;
//     vnidx->pre_link=vnidx->next_link=next_index;
//     vnidx->refcnt=1;

//     vnidx->mode=TYPE_LINK;
//     double_link_add(vinodes[vidx->father_dir].child,next_index);

//     father_dir=next_index;
//   }else{
//     assert(dir_index!=-1&&father_dir!=-1);
//     assert(voidx->next==-1);

//     voidx->next=next_index;

//     strcpy(vnidx->name,buf.name);
//     strcpy(vnidx->path,vidx->path);
//     strcat(vnidx->path,buf.name);

//     if(buf.mode&TYPE_DIR){
//       strcat(vnidx->path,"/");
//     }
//     printf("vnidx->path: %s\n",vnidx->path);

//     vnidx->dir=dir_index;
//     vnidx->father_dir=father_dir;

//     vnidx->next=-1;
//     vnidx->child=-1;

//     vnidx->pre_link=vnidx->next_link=next_index;
//     vnidx->refcnt=1;

//     vnidx->mode=buf.mode;
//   }

//   vnidx->rinode_index=buf.rinode_index;
//   vnidx->fs_type=vidx->fs_type;
//   vnidx->fs=vidx->fs;
//   origin_index=next_index;


//   if(check_item_match(buf.name,path+offset,file_len)){
//     assert(next_inode==-1);
//     next_inode=next_index;
    
//   }

// }
// if(next_inode==-1){
//   printf("Error: Directory exists, but can't find fild!\n");
//   return -1;
// }

//   int noffset=1;
//   index=(path[0] == '/') ? lookup_root(path, &flag, &noffset)
//                          : lookup_cur(path, &flag, VFS_ROOT, &noffset);
//   assert(noffset>offset);
//   return (noffset == offset) ? -1 : vinode_lookup(path);
int len = strlen(path);
  if (path[len - 1] == '/') path[len - 1] = '\0';

  int flag, offset = 1;
  int index = (path[0] == '/') ? lookup_root(path, &flag, &offset)
                             : lookup_cur(path, &flag, VFS_ROOT, &offset);
  if (flag == 1) return index;

  vinode_t buf;
  int kth = 0, origin_index = -1, next_index = -1;
  int dir = -1, father_dir = -1, ret = -1, next = -1;

  int flen = first_name_len(path + offset);
  // printf("%s, %d\n", path + offset, flen);

  if (vidx->fs == NULL) return -1;

  if (vidx->child != -1) return -1;
  printf("rinode: %d\n",vidx->rinode_index);

  while ((ret = vidx->fs->readdir(vidx->fs, vidx->rinode_index, ++kth, &buf))) {
    if ((next_index = vit_item_alloc()) == -1) assert(0);

    if (!strcmp(buf.name, ".")) {
      assert(origin_index == -1);

      vidx->child = next_index;

      strcpy(vnidx->name, ".");
      strcpy(vnidx->path, vidx->path);
      vnidx->dir = -1, vnidx->father_dir = -1;  // will be cover
      vnidx->next = -1, vnidx->child = index;
      vnidx->pre_link = vnidx->next_link = next_index, vnidx->refcnt = 1;
      vnidx->mode = TYPE_LINK, double_link_add(index, next_index);

      dir = next_index;
    } else if (!strcmp(buf.name, "..")) {
      assert(voidx->next == -1);
      voidx->next = next_index;
      voidx->father_dir = next_index;
      strcpy(vnidx->name, "..");
      strcpy(vnidx->path, vinodes[vinodes[vidx->dir].child].path);
      vnidx->dir= origin_index, vnidx->father_dir = -1;
      vnidx->next = -1, vnidx->child = vinodes[vidx->father_dir].child;
      vnidx->pre_link = vnidx->next_link = next_index, vnidx->refcnt = 1;
      vnidx->mode = TYPE_LINK, double_link_add(vinodes[vidx->dir].child, next_index);

      father_dir = next_index;
    } else {
      printf("dir: %d father_dir:%d\n",dir,father_dir);
      assert(dir != -1 && father_dir != -1);
      assert(voidx->next == -1);
      voidx->next = next_index;
      strcpy(vnidx->name, buf.name);
      strcpy(vnidx->path, vidx->path);
      strcat(vnidx->path, buf.name);
      if (buf.mode & TYPE_DIR) strcat(vnidx->path, "/");
      vnidx->dir = dir, vnidx->father_dir = father_dir;
      vnidx->next = -1, vnidx->child = -1;
      vnidx->pre_link = vnidx->next_link = next_index, vnidx->refcnt = 1;
      vnidx->mode = buf.mode;
    }

    vnidx->rinode_index = buf.rinode_index;
    vnidx->fs_type = vidx->fs_type;
    vnidx->fs = vidx->fs;
    origin_index = next_index;

    if (check_item_match(buf.name, path + offset, flen)) {
      assert(next == -1);
      next = next_index;
    }
  }

  if (next == -1) {
    printf("read directory, but file is not exists!\n");
    return -1;
  }

  int noffset = 1;
  index = (path[0] == '/') ? lookup_root(path, &flag, &noffset)
                         : lookup_cur(path, &flag, VFS_ROOT, &noffset);
  assert(noffset > offset);
  return (noffset == offset) ? -1 : vinode_lookup(path);
}
//read/write 操作: vinodes 和 fildes　两个结构体的联动;
static int flides_alloc(){
//返回一个可用的节点;;
int i;
int  flag=0;
  for(i=0;i<MAX_FILE_NUM;i++){
    if(flides[i].refcnt==0){
    flag=1;
      break;
    }
  }
if(flag==1){

  printf("flag==1,i=:%d\n",i);
  return i;
}else{return -1;};//-1表示没有空位了;

//  return 0;
}
// static void flides_free(int index){
//   flides[index].refcnt=0;
//   flides[index].open_offset=0;
//   flides[index].vinode_index=0;
//   return;
// }

static int flides_open(int index,uint32_t rwmode){
  //index 是vinodes结构体里面的编号,mode调控只读只写;

  //To be continued;
  
  int fd=flides_alloc();
  if(fd==-1){
    return -1;//没有多余的flides空位了;
  }

  flides[fd].refcnt+=1;
  flides[fd].open_offset=0;
  flides[fd].vinode_index=index;
  switch (rwmode)
  {
  case O_RDONLY:
    flides[fd].read=1;
    flides[fd].write=0;
    flides[fd].rw=0;
    break;
  case O_WRONLY:
    flides[fd].read=0;
    flides[fd].write=1;
    flides[fd].rw=0;
    break;
  case O_RDWR:
    flides[fd].read=0;
    flides[fd].write=0;
    flides[fd].rw=1;
    break;
  
  default:
    return -1;
    break;
  }
  return fd;
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
// static int filesystem_free(int index){
//   strcpy(filesystems[index].name,"");//清空名字;
//   return 0;
// }

//devvfs:
#define fidx (&filesystems[index])
static int vfs_init_devfs(const char *name, device_t *dev, size_t size,
                             void (*init)(filesystem_t *, const char *,
                                          device_t *),
                             int (*readdir)(filesystem_t *, int, int,
                                            vinode_t *)){
  int index=filesystem_alloc();
  fidx->real_fs=pmm->alloc(size);
  fidx->init=init;
  fidx->readdir=readdir;
  fidx->dev=dev;
  strcpy(fidx->name,name); 
  fidx->init(fidx,fidx->name,fidx->dev);
  return index;


}
void vinode_prepare(int index,
  int rinode_index,int dir,int father_dir,int next,int child,
  int next_link,int pre_link,int refcnt,int mode,
  int fs_type,filesystem_t*fs,
  char *name,char *path
){
  vidx->rinode_index=rinode_index;
  vidx->dir=dir;
  vidx->father_dir=father_dir;
  vidx->next=next;
  vidx->child=child;
  vidx->next_link=next_link;
  vidx->pre_link=pre_link;
  vidx->refcnt=refcnt;
  vidx->mode=mode;
  vidx->fs_type=fs_type;
  vidx->fs=fs;
  strcpy(vidx->name,name);
  strcpy(vidx->path,path);
}



void vinode_root_prepare(int dir,int index){
  vinode_prepare(index,-1,-1,-1,-1,dir,index,index,1,TYPE_DIR,VFS,NULL,"/","/");
}
void vinode_dot_prepare(int dir,int index,int father_dir){
  vinode_prepare(dir,-1,-1,father_dir,father_dir,index,dir,dir,1,TYPE_LINK
  ,VFS,NULL,".",vidx->path);
}
void vinode_ddot_prepare(int dir,int index,int father_dir,int par){
  vinode_prepare(father_dir,-1,dir,-1,-1,par,father_dir,
  father_dir,1,TYPE_LINK,VFS,NULL,"..",vidx->path);
}
char path_buf[MAX_PATH_LENGTH];
static int root_dir_prepare(){
  int index=vit_item_alloc();
  int dir=vit_item_alloc();
  int father_dir=vit_item_alloc();
  int par=index;

  if(index!=VFS_ROOT){
    assert(0);
  }  

  // vidx->rinode_index=-1;
  // vidx->dir=-1;
  // vidx->father_dir=-1;
  // vidx->next=-1;
  // vidx->child=dir;
  // vidx->next_link=vidx->pre_link=index;
  // vidx->refcnt=1;
  // vidx->mode=TYPE_DIR;
  // vidx->fs_type=VFS;
  // vidx->fs=NULL;
  // strcpy(vidx->name,"/");
  // strcpy(vidx->path,"/");
  vinode_root_prepare(dir,index);
//"."初始化;
  // strcpy(vdir->name,".");
  // strcpy(vdir->path,vidx->path);
  // vdir->dir=-1;
  // vdir->father_dir=father_dir;
  // vdir->next=father_dir;
  // vdir->child=index;
  // vdir->pre_link=vdir->next_link=dir;
  // vdir->refcnt=1;
  // vdir->mode=TYPE_LINK;
  vinode_dot_prepare(dir, index, father_dir);
  double_link_add(index,dir);
  // vdir->fs_type=VFS;
  // vdir->fs=NULL;
//".."初始化;
  // strcpy(vfat->name,"..");
  // strcpy(vfat->path,vidx->path);
  // vfat->dir=dir;
  // vfat->father_dir=-1;
  // vfat->next=-1;
  // vfat->child=index;
  // vfat->pre_link=vdir->next_link=father_dir;
  // vfat->refcnt=1;
  // vfat->mode=TYPE_LINK;
  vinode_ddot_prepare( dir, index, father_dir,par);
  double_link_add(par,father_dir);
  // vfat->fs_type=VFS;
  // vfat->fs=NULL;

  return index;

}


static int append_dir(int par,char *name,int mode,int fs_type,filesystem_t *fs){
  int next_index=vit_item_alloc();
  int k=vinodes[par].child;
  int dir=-1,father_dir=-1;

  //找到最后一个节点;
  for(;vinodes[k].next!=-1;k=vinodes[k].next){
    if (!strcmp(vinodes[k].name, ".")) {//找到".""记录当前dir以及father_dir;
    dir=k;
    father_dir=vinodes[k].next;
  }
  }
  assert(dir != -1 && father_dir != -1);
  vinodes[k].next=next_index;
  //目录初始化;
  strcpy(vnidx->name,name);
  strcpy(vnidx->path,vdir->path);
  strcat(vnidx->path,name);
  strcat(vnidx->path,"/");
  vinode_prepare(next_index,-1,dir,father_dir,-1,-1,next_index,
 next_index,1,mode,fs_type,fs,vnidx->name,vnidx->path);
  // vnidx->dir=dir;
  // //vnidx->dir=dir;
  // vnidx->father_dir=father_dir;
  // vnidx->next=-1;
  // vnidx->child=-1;
  // vnidx->pre_link=vdir->next_link=next_index;
  // vnidx->refcnt=1;
  // vnidx->mode=mode;
  // //double_link_add(index,father_dir);
  // vnidx->fs_type=fs_type;
  // vnidx->fs=fs;
  
  
  return next_index;

}

static int append_file(int par, char *name, int mode, int fs_type,
                       filesystem_t *fs){
  int next_index=vit_item_alloc();
  int k=vinodes[par].child;
  int dir=-1;
  int father_dir=-1;

  assert(k!=-1);

  for (; vinodes[k].next != -1; k = vinodes[k].next) {
    if (!strcmp(vinodes[k].name, ".")) {
      dir = k;
      father_dir = vinodes[k].next;
    }
  }
  assert(dir != -1 && father_dir != -1);
  vinodes[k].next=next_index;
  strcpy(vnidx->name,name);
  strcpy(vnidx->path,vdir->path);
  strcat(vnidx->path,name);
  vinode_prepare(next_index,-1,dir,father_dir,-1,-1,next_index,
  next_index,1,mode,fs_type,fs,vnidx->name,vnidx->path);

  return next_index;
}
static int vfs_dir_prepare(int index, int par, int fs_type, filesystem_t *fs){
  int dir=vit_item_alloc();
  int father_dir=vit_item_alloc();

  assert(vidx->child==-1);//子节点为空;
  vidx->child=dir;

  //build_dot(index, fs_type, fs);
  //build_ddot(par, fs_type, fs);
   vinode_dot_prepare(dir, index, father_dir);
  double_link_add(index,dir);
   vinode_ddot_prepare( dir, index, father_dir,par);
  double_link_add(par,father_dir);
  return dir;

}

static void vinode_delete(int index){
  vit_item_free(index);
  double_link_remove(index);
}
static int vfs_dir_remove(int index, int par){//在par目录删除index目录
//首先找到index,链接补全,删除index的. ..软链接,然后在par里面找到index进行删除;
int temp_index=vinodes[par].child;
while(1){
  if(vinodes[temp_index].next==index)break;
  temp_index=vinodes[temp_index].next;
}
vinodes[temp_index].next=vinodes[index].next;

//删除dir的所有节点;
int k=vidx->child;
printf("vinode[%d].name: %s",k,vinodes[k].name);
int next=vinodes[k].next;
while(1){
 printf("vinode[%d].name: %s,next:%d\n",k,vinodes[k].name,vinodes[k].next);
  next=vinodes[k].next;
  vinode_delete(k);
  if(next==-1)break;
  k=next;
}

vinode_delete(index);
return 0;

}
static int vfs_file_remove(int index,int par){
  int temp_index=vinodes[par].child;
  while(1){
    if(vinodes[temp_index].next==index)break;
    temp_index=vinodes[temp_index].next;
  }
  vinodes[temp_index].next=vinodes[index].next;

  vinode_delete(index);
  return 0;



}

void vfs_info(){
  for(int i=0;i<MAX_VINODE_NUM;i++){
    if(vinodes[i].mode!=UNUSED){
      printf("%d name: %s path:%s next:%d next_link:%d pre_link:%d,type:%d,rinode:%d, child:%d \n",i,
      vinodes[i].name,vinodes[i].path,vinodes[i].next,vinodes[i].next_link,vinodes[i].pre_link,
      vinodes[i].mode,vinodes[i].rinode_index,vinodes[i].child);
    }
  }
}
  char *vfs_real_path(const char *path){
    strcpy(path_buf,path);
    int index=vinode_lookup(path_buf);
    return vidx->path;//忽略软链接;

  }
extern int ext2_readdir(filesystem_t *fs,int rinode_idx,int kth,vinode_t * buf);
extern void ext2_init(fs_t * fs,const char * name ,device_t* dev);
extern int ext2_create(ext2_t* ext2, int ridx, char* name, int mode);
extern int ext2_remove(ext2_t* ext2,int index,char* name,int mode);
extern ssize_t ext2_read(ext2_t* ext2, int index, uint64_t offset, char* buf,
                  uint32_t len);
extern ssize_t ext2_write(ext2_t * ext2,int index,uint64_t offset,char * buf,
                  uint32_t len);
  void vfs_init(){
   // int success=vinode_lookup("/");
    //assert(success!=-1);
    int root=root_dir_prepare();
    if(root==-1){
      assert(0);
    }

    for(int i=0;i<MAX_FILE_NUM;i++){
      flides[i].refcnt=0;
    }
    int dev=append_dir(root,"dev",TYPE_DIR,VFS,NULL);
    int mnt=append_dir(root,"mnt",TYPE_DIR,VFS,NULL);
    int r0fs=vfs_init_devfs("ramdisk0",dev_lookup("ramdisk0"),sizeof(ext2_t)
                             ,ext2_init,ext2_readdir);
    int r1fs=vfs_init_devfs("ramdisk1",dev_lookup("ramdisk1"),sizeof(ext2_t)
                             ,ext2_init,ext2_readdir);

    vfs_dir_prepare(dev,root,VFS,NULL);
    vfs_dir_prepare(mnt,root,VFS,NULL);

   int ramdisk0= append_dir(dev, "ramdisk0", TYPE_DIR | MNT_ABLE, EXT2FS, &filesystems[r0fs]);
    vinodes[ramdisk0].rinode_index=EXT2_ROOT;
    //vfs_dir_prepare(ramdisk0,dev,VFS,NULL);
    append_file(dev, "ramdisk1", TYPE_FILE | MNT_ABLE, EXT2FS, &filesystems[r1fs]);

    append_file(dev, "tty1", TYPE_FILE | WR_ABLE, TTY, NULL);
    append_file(dev, "tty2", TYPE_FILE | WR_ABLE, TTY, NULL);
    append_file(dev, "tty3", TYPE_FILE | WR_ABLE, TTY, NULL);
    append_file(dev, "tty4", TYPE_FILE | WR_ABLE, TTY, NULL);



    return ;
  };
  int vfs_access(const char *path, int mode){//如果符合应该return 0;
    strcpy(path_buf,path);
    printf("path_buf:%s",path_buf);
    int index=vinode_lookup(path_buf);
    printf("index:%d",index);
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
  char tempbuff[MAX_PATH_LENGTH];
  static void get_father_dir(const char*path,int offset){
    strcpy(tempbuff,path);
    tempbuff[offset]='\0';
  }
  int vfs_mkdir(const char *path){//mkdir 的path 应该是包含待创建目录的;
//首先找到父目录的index,然后创建就可以了;
  //只允许在ext2里面建;

  int father_dir_offset=path_length_offset(path);
  int path_len=strlen(path);
  if(father_dir_offset==path_len){
    return -1;//错误１: 目录格式错误;
  }
  get_father_dir(path,father_dir_offset);
  int index=vinode_lookup(tempbuff);
  int next_index=-1;
  int rinode_index=-1;
  if(vidx->fs_type==EXT2FS){
    
    rinode_index=ext2_create(vidx->fs->real_fs,vidx->rinode_index,tempbuff+father_dir_offset+1,TYPE_DIR);
    next_index=append_dir(index,tempbuff+father_dir_offset+1,TYPE_DIR,vidx->fs_type,vidx->fs);
    vfs_dir_prepare(next_index,index,vidx->fs_type,vidx->fs);
    vinodes[next_index].rinode_index=rinode_index;
  }else{
    return -2;//错误２:　ext2fs之外建立目录;
  }
  
    return 0;
  };
  int vfs_rmdir(const char *path){
//找到父亲目录的inode,然后进行删除;   
  int father_dir_offset=path_length_offset(path); 
    int path_len=strlen(path);
  if(father_dir_offset==path_len){
    return -1;//错误１: 目录格式错误;
  }
  strcpy(tempbuff,path);
  int now_index=vinode_lookup(tempbuff);//得到当前的inode;
  get_father_dir(path,father_dir_offset);
  int index=vinode_lookup(tempbuff);//得到父亲节点的inode;
  int mode=TYPE_DIR;
  if(vidx->fs_type==EXT2FS){
    if(ext2_remove(vidx->fs->real_fs,vidx->rinode_index,tempbuff+father_dir_offset+1,mode)==0){
      vfs_dir_remove(now_index,index);
    }
    else{
      return -2;// 没办法在ext2fs里面移除;
    }
  }else{
    return -3;//文件系统不能删除;
  }

 // printf("now: %d, father: %d",now_index,index);
    return 0;
  };
  int vfs_create_file(const char* path){
  int father_dir_offset=path_length_offset(path);
  int path_len=strlen(path);
  if(father_dir_offset==path_len){
    return -1;//错误１:　文件格式错误;
  }
  get_father_dir(path,father_dir_offset);
  int index=vinode_lookup(tempbuff);
  int next_index=-1;
  int rinode_index=-1;

   if(vidx->fs_type==EXT2FS){
    //May be a bug!!!!!!!!!!
    rinode_index=ext2_create(vidx->fs->real_fs,vidx->rinode_index,tempbuff+father_dir_offset+1,TYPE_FILE|WR_ABLE|RD_ABLE);
    next_index=append_file(index,tempbuff+father_dir_offset+1,TYPE_FILE,vidx->fs_type,vidx->fs);
    //vfs_dir_prepare(next_index,index,vidx->fs_type,vidx->fs);
    vinodes[next_index].rinode_index=rinode_index;
  }else{
    return -2;//错误２:　ext2fs之外建立文件;;
  }
    return 0;
  }

int vfs_remove_file(const char *path){
//找到父亲目录的inode,然后进行删除;   
  int father_dir_offset=path_length_offset(path); 
    int path_len=strlen(path);
  if(father_dir_offset==path_len){
    return -1;//错误１: 目录格式错误;
  }
  strcpy(tempbuff,path);
  int now_index=vinode_lookup(tempbuff);//得到当前的inode;
  //printf("now_index:%d",now_index);
  get_father_dir(path,father_dir_offset);
  int index=vinode_lookup(tempbuff);//得到父亲节点的inode;
  int mode=TYPE_FILE;
  if(vidx->fs_type==EXT2FS){
    if(ext2_remove(vidx->fs->real_fs,vidx->rinode_index,tempbuff+father_dir_offset+1,mode)==0){
      vfs_file_remove(now_index,index);//记得修改;
    }
    else{
      return -2;// 没办法在ext2fs里面移除;
    }
  }else{
    return -3;//文件系统不能删除;
  }

 // printf("now: %d, father: %d",now_index,index);
    return 0;

}

  int vfs_link(const char *oldpath, const char *newpath){
    return 0;
  };
  int vfs_unlink(const char *path){
    return 0;
  };
  int vfs_open(const char *path, int rwmode){
    if(vfs_access(path,TYPE_FILE)==0){
      strcpy(tempbuff,path);
      int index=vinode_lookup(tempbuff);
      int fd=flides_open(index,rwmode);
      printf("vfs_open: fd:%d\n",fd);

      return fd;
    }else{
      return -1;//找不到文件;
    }

    return -1;
  }
  ssize_t vfs_read(int fd, void *buf, size_t nbyte){

    int result=-1;
    if(flides[fd].refcnt==0){
      return -1;
    } 
    int index=flides[fd].vinode_index;
    int fs_type=vidx->fs_type;
    int rinode=vidx->rinode_index;
    //printf("vfs_read:index:%d\n",index);

    switch (fs_type)
    {
    case EXT2FS:
      result=ext2_read(vidx->fs->real_fs,rinode,flides[fd].open_offset,buf,nbyte);
      flides[fd].open_offset+=result;
      break;
    
    default:
      break;
    }


    return result;
  }
  ssize_t vfs_write(int fd, void *buf, size_t nbyte){
    int result=-1;
    if(flides[fd].refcnt==0){
      return -1;
    } 
    int index=flides[fd].vinode_index;
    int fs_type=vidx->fs_type;
    int rinode=vidx->rinode_index;

    switch (fs_type)
    {
    case EXT2FS:
      result=ext2_write(vidx->fs->real_fs,rinode,flides[fd].open_offset,buf,nbyte);
      flides[fd].open_offset+=result;
      break;
    
    default:
      break;
    }

    return result;
  }
  off_t vfs_lseek(int fd, off_t offset, int whence){
    return 0;
  }
  int vfs_close(int fd){
    return 0;
  }
  // int vfs_remove(const char *path){
  //   return 0;
  // }
  // int vfs_create(const char *path){
  //   return 0;
  // }

  void vfs_ls(char * dir,char *outbuf){
  int index=vinode_lookup(dir);
  if (index == -1) return;
  int offset=sprintf(outbuf,"In vinode[%d] name:%s path:%s\n",index,vinodes[index].name,vinodes[index].path);
    for (int k = vinodes[index].child; k != -1; k = vinodes[k].next) {
      offset+=sprintf(outbuf+offset,"inode:%d \tdirname:%s \tpath:%s\n",k,vinodes[k].name,vinodes[k].path);
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
    //.create = vfs_create,
    //.remove = vfs_remove,
    .link = vfs_link,
    .unlink = vfs_unlink,
    .open = vfs_open,
    .read = vfs_read,
    .write = vfs_write,
    .lseek = vfs_lseek,
    .close = vfs_close,
    .create_file = vfs_create_file,
    //.remove_file=vfs_remove_file,
};

