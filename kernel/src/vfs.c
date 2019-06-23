#include <blkfs.h>
#include <vfs.h>



//全局变量;
vinode_t vit[MAX_VINODE_NUM];//vfs inode table
filesystem_t fs_table[MAX_FS_NUM];//filesystem table;

//下面是关于vinode_table操作的函数;
static int vit_init();
static int vit_item_alloc();//在vinode_table里面找到一个空块返回一个index;
static void vit_item_free(int index);//在vinode_table 里面释放这个空块;
static int vit_lookup_root(char *name);//从根目录开始遍历找;如果不存在返回-1;
static int vit_lookup_auto(char *path);//自动找path;
static int vit_lookup_cur(char *name,int check,int dir_index);//在index=dir_index的目录找name匹配的文件;
static int vit_init(){
  for (int i = 0;i<MAX_VINODE_NUM;i++){
    vit[i].exist=0;//不存在;
  }


  return 0;
}

static int vit_item_alloc(){
  for (int i=0;i<MAX_VINODE_NUM;i++){
    if(vit[i].exist==0){
      vit[i].exist=1;
      return i;
    }
  }
  printf("Error： The vfs inode table is full!");
  return -1;
}

static void vit_item_free(int index){
  vit[index].exist=0;//不存在;
}

static int vit_lookup_cur(char *name,int check,int dir_index){
return 0;
}

static int vit_lookup_root(char *name)//从根目录开始遍历找;如果不存在返回-1;
{
  return 0 ;
}
static int vit_lookup_auto(char *path)//自动找path;
{
  return 0;
}










  void vfs_init();
  int vfs_access(const char *path, int mode);
  int vfs_mount(const char *path, filesystem_t *fs);
  int vfs_unmount(const char *path);
  int vfs_mkdir(const char *path);
  int vfs_rmdir(const char *path);
  int vfs_link(const char *oldpath, const char *newpath);
  int vfs_unlink(const char *path);
  int vfs_open(const char *path, int flags);
  ssize_t vfs_read(int fd, void *buf, size_t nbyte);
  ssize_t vfs_write(int fd, void *buf, size_t nbyte);
  off_t vfs_lseek(int fd, off_t offset, int whence);
  int vfs_close(int fd);


  void vfs_init(){
    vit_init();
    return ;
  }

MODULE_DEF(vfs) {
  .init   = vfs_init,
  .access=vfs_access,
  .mount=vfs_mount,
  .unmount=vfs_unmount,
  .mkdir=vfs_mkdir,
  .rmdir=vfs_rmdir,
  .link=vfs_link,
  .unlink=vfs_unlink,
  .open=vfs_open,
  .read=vfs_read,
  .write=vfs_write,
  .lseek=vfs_lseek,
  .close=vfs_close,
};



  int vfs_access(const char *path, int mode);
  int vfs_mount(const char *path, filesystem_t *fs);
  int vfs_unmount(const char *path);
  int vfs_mkdir(const char *path);
  int vfs_rmdir(const char *path);
  int vfs_link(const char *oldpath, const char *newpath);
  int vfs_unlink(const char *path);
  int vfs_open(const char *path, int flags);
  ssize_t vfs_read(int fd, void *buf, size_t nbyte);
  ssize_t vfs_write(int fd, void *buf, size_t nbyte);
  off_t vfs_lseek(int fd, off_t offset, int whence);
  int vfs_close(int fd);



  int vfs_access(const char *path, int mode){
    assert(0);
    return 0;
  }
  int vfs_mount(const char *path, filesystem_t *fs){
    assert(0);
    return 0;
  }
  int vfs_unmount(const char *path){
    assert(0);
    return 0;
    ;
  }

  int vfs_mkdir(const char *path){
    assert(0);
    return 0;

  }
  int vfs_rmdir(const char *path){
    assert(0);
    return 0;

  }
  int vfs_link(const char *oldpath, const char *newpath){
    assert(0);
    return 0;

  }
  int vfs_unlink(const char *path){
    assert(0);
    return 0;

  }
  int vfs_open(const char *path, int flags){
    //assert(0);
    return 0;
  }
  ssize_t vfs_read(int fd, void *buf, size_t nbyte){
    assert(0);
    return 0;

  }
  ssize_t vfs_write(int fd, void *buf, size_t nbyte){
    assert(0);
    return 0;

  }
  off_t vfs_lseek(int fd, off_t offset, int whence){
    assert(0);
    return 0;

  }
  int vfs_close(int fd){
    assert(0);
    return 0;

  }





  void ext2ops_init(struct filesystem *fs, const char *name, dev_t *dev){
    ext2_init(fs,name,dev);
  }
  //inode_t *ext2ops_lookup(struct filesystem *fs, const char *path, int flags){;}




