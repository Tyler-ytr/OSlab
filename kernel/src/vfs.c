// #include <blkfs.h>
// #include <vfs.h>



// //全局变量;
// vinode_t vit[MAX_VINODE_NUM];//vfs inode table
// filesystem_t fs_table[MAX_FS_NUM];//filesystem table;

// //下面是关于vinode_table操作的函数;
// static int vit_init();
// static int vit_item_alloc();//在vinode_table里面找到一个空块返回一个index;
// static void vit_item_free(int index);//在vinode_table 里面释放这个空块;
// //static int vit_lookup_root(char *path);//从根目录开始遍历找;如果不存在返回-1;
// static int vit_lookup_root(char *path,int * check);
// //static int vit_lookup_auto(char *path);//自动找path;
// static int vit_lookup_cur(char *path,int* check,int dir_index);//在index=dir_index的目录找name匹配的文件;
// static int vit_init(){
//   for (int i = 0;i<MAX_VINODE_NUM;i++){
//     vit[i].exist=0;//不存在;
//     vit[i].next=-1;
//     vit[i].prelink=vit[i].nextlink=i;
//   }

  
//   return 0;
// }

// static int vit_item_alloc(){
//   for (int i=0;i<MAX_VINODE_NUM;i++){
//     if(vit[i].exist==0){
//       vit[i].exist=1;
//       return i;
//     }
//   }
//   printf("Error： The vfs inode table is full!");
//   return -1;
// }

// static void vit_item_free(int index){
//   vit[index].exist=0;//不存在;
// }
// static int first_name_len(char *name){//返回/前面的第一个名字;
//   int name_len=strlen(name); 
//   int result=0;
//   while(result<name_len&&(name[result]!='/')){
//     result+=1;
//   }
//   return result;

// }
// static int vit_lookup_cur(char *path,int* check,int dir_index){//从cur这个目录开始遍历inode_list链表;
//   if(strlen(path)==0){
//     ///printf("None path in vit_lookup_cur");
//     *check=1;
//     //找到了;
//     return dir_index;
//   }

//   int name_len=first_name_len(path);
//   int success=0;
//   for(int i=vit[dir_index].child;i!=-1;i=vit[dir_index].next){
//     if(strncmp(vit[i].path,path,name_len)==0){
//       printf("In vit_lookup_cur: Match!\n");
//       success=i;
//       break;
//     }
//   }

//   if(success!=0){
//     ;
//   }
//   else{
//     *check=0;
//     //没找到;
//     return dir_index;
//   }
//   //目前 ....../a/b/c的.....找到了; 需要判断文件格式;如果是目录,下一个,如果是文件,下一个,会在之前的那个if判断返回;,如果是软链接,下一个;

//   int next=success;
//   while(vit[next].mode==TYPE_LINK){
//     printf("In vit_look_up: link find, from :%s -> %s",vit[next].path,vit[vit[next].nextlink].path);
//     next=vit[next].nextlink;
//   }

//   char *next_path=path+(name_len+(path[name_len]=='/'?1:0));


//   return vit_lookup_cur(next_path,check,next);
// }

// static int vit_lookup_root(char *path,int * check){
//   return vit_lookup_cur(path+1,check,0);//0是根目录的inode;
// }
// static int vit_lookup_real(char *path,int * check){//调用底层的fs并建立树结构;




// return 0;

// }


// //static int vit_lookup_root(char *name)//从根目录开始遍历找;如果不存在返回-1;
// //{
//  // return 0 ;
// //}
// //static int vit_lookup_auto(char *path)//自动找path;
// //{
// //  return 0;
// //}











// extern void ext2_init(fs_t * fs,const char * name ,device_t* dev);
// extern uint32_t ext2_research_file(ext2_t *ext2,char *name,int file_type,
//                            uint32_t * inode_num,uint32_t* block_num,uint32_t* dir_num);




//   void vfs_init();
//   int vfs_access(const char *path, int mode);
//   int vfs_mount(const char *path, filesystem_t *fs);
//   int vfs_unmount(const char *path);
//   int vfs_mkdir(const char *path);
//   int vfs_rmdir(const char *path);
//   int vfs_link(const char *oldpath, const char *newpath);
//   int vfs_unlink(const char *path);
//   int vfs_open(const char *path, int flags);
//   ssize_t vfs_read(int fd, void *buf, size_t nbyte);
//   ssize_t vfs_write(int fd, void *buf, size_t nbyte);
//   off_t vfs_lseek(int fd, off_t offset, int whence);
//   int vfs_close(int fd);


//   void vfs_init(){
//     vit_init();

//     return ;
//   }

// MODULE_DEF(vfs) {
//   .init   = vfs_init,
//   .access=vfs_access,
//   .mount=vfs_mount,
//   .unmount=vfs_unmount,
//   .mkdir=vfs_mkdir,
//   .rmdir=vfs_rmdir,
//   .link=vfs_link,
//   .unlink=vfs_unlink,
//   .open=vfs_open,
//   .read=vfs_read,
//   .write=vfs_write,
//   .lseek=vfs_lseek,
//   .close=vfs_close,
// };



//   int vfs_access(const char *path, int mode);
//   int vfs_mount(const char *path, filesystem_t *fs);
//   int vfs_unmount(const char *path);
//   int vfs_mkdir(const char *path);
//   int vfs_rmdir(const char *path);
//   int vfs_link(const char *oldpath, const char *newpath);
//   int vfs_unlink(const char *path);
//   int vfs_open(const char *path, int flags);
//   ssize_t vfs_read(int fd, void *buf, size_t nbyte);
//   ssize_t vfs_write(int fd, void *buf, size_t nbyte);
//   off_t vfs_lseek(int fd, off_t offset, int whence);
//   int vfs_close(int fd);



//   int vfs_access(const char *path, int mode){
//     assert(0);
//     return 0;
//   }
//   int vfs_mount(const char *path, filesystem_t *fs){
//     assert(0);
//     return 0;
//   }
//   int vfs_unmount(const char *path){
//     assert(0);
//     return 0;
//     ;
//   }

//   int vfs_mkdir(const char *path){
//     assert(0);
//     return 0;

//   }
//   int vfs_rmdir(const char *path){
//     assert(0);
//     return 0;

//   }
//   int vfs_link(const char *oldpath, const char *newpath){
//     assert(0);
//     return 0;

//   }
//   int vfs_unlink(const char *path){
//     assert(0);
//     return 0;

//   }
//   int vfs_open(const char *path, int flags){
//     //assert(0);
//     return 0;
//   }
//   ssize_t vfs_read(int fd, void *buf, size_t nbyte){
//     assert(0);
//     return 0;

//   }
//   ssize_t vfs_write(int fd, void *buf, size_t nbyte){
//     assert(0);
//     return 0;

//   }
//   off_t vfs_lseek(int fd, off_t offset, int whence){
//     assert(0);
//     return 0;

//   }
//   int vfs_close(int fd){
//     assert(0);
//     return 0;

//   }





//   void ext2ops_init(struct filesystem *fs, const char *name, dev_t *dev){
//     ext2_init(fs,name,dev);
//   }
//   //inode_t *ext2ops_lookup(struct filesystem *fs, const char *path, int flags){;}
#include <vfs.h>
#include <blkfs.h>

  void vfs_init(){
    return;
  };
  int vfs_access(const char *path, int mode){
    return 0;
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

