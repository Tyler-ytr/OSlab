#ifndef _VFS_H
#define _VFS_H
#include <common.h>
typedef struct fsops fsops_t;
typedef struct indeops indeops_t;
typedef struct vinode vinode_t;
typedef struct vinodeops vinodeops_t;
typedef struct mount_path_point mpp_t;
typedef struct filesystem fs_t;
typedef struct file file_t;

#define MAX_FILE_NAME 64
#define MAX_PATH_LENGTH 256
#define MAX_NAME_LENGTH 64
#define MAX_FILE_NUM 128
#define MAX_VINODE_NUM 256
#define MAX_FS_NUM 4

#define VFS 0x00
#define EXT2FS 0x01
#define PROCFS 0x02
#define TTY 0x04

#define UNUSED 0x0000
#define EX_ABLE 0x0001
#define WR_ABLE 0x0002
#define RD_ABLE 0x0004
#define TYPE_FILE 0x0010
#define TYPE_DIR 0x0020
#define TYPE_LINK 0x0040
#define MNT_ABLE 0x0100
#define UNMNT_ABLE 0x0200
#define ALLOCED 0x80000

#define O_RDONLY 0x0004
#define O_WRONLY 0x0002
#define O_RDWR 0x0001


#define VFS_ROOT 0
#define EXT2_ROOT 1
#define PROCFS_ROOT -1
//enum SYS_TYPE{VFS=0x00,BLKFS=0x01,PROCFS=0x02};
//enum ROOT_LOCATION{VFS_ROOT=0,EXT2_ROOT=1,PROCFS_ROOT=-1};


//enum FILE_TYPE{TYPE_FILE=0,TYPE_DIR=1,TYPE_LINK=2};
struct vinode {//暂时只允许单级cd;
  char path[MAX_PATH_LENGTH]; //存放绝对路径;
  char name[MAX_NAME_LENGTH]; //存放名字;
  int exist;        //这个块活着吗?在vit_item_free里面用到;
//服务于树结构;  
  int dir;          //type= “文件” 所属的目录项在vinode里面的编号,如果type="目录",那么就是它自身;
  int father_dir;   //所属的当前目录项的父亲目录项,也就是当前目录项的".."对应的vinode里面的index;
  int child;//不是目录属性的就没有;
  int next;//专属于文件属性,最后的文件是-1;
  uint32_t mode;

  int refcnt;      //次数,link,unlink维护;

  uint32_t rinode_index;//存储实际文件系统的编号;仅仅适用于ext32; 可能profs,devfs也能用index编号来处理;
  uint32_t size;   //文件总大小;在write的时候会发生改变;
  filesystem_t *fs;//从属的文件系统;
  int fs_type;    //和vfs.h中的filesystem的type对应;
//  int type;       //文件格式; file为0,dir为1,link为2;

  //服务于link的双向链表;
  int pre_link;
  int next_link;
  //一开始的真实文件prelink,nextlink都是自身;加入一个link a之后 original.nextlink=a,a.prelink=original; 
  //prelink是自身的保证是真实的文件;
  //只读只写等操作,随用随加;

 // vinodeops_t *vinodeops;//我不太会写,可能我之后会拆开;
};
//在vfs.c里面开一个全局的vinode_t数组,假设名叫 vinode_table;
//vinode 函数;



/*
struct vinodeops {
  int (*open)(int index, int flags);//用vit_lookup(char *name)找到相应的index，调用open,就会在当前的task.flides表里面找到一个可用的块,并且做好初始化，返回fd;
  int (*close)(int fd);       //根据fd在flides表里面删除这一块;
  ssize_t (*read)(int fd, char *buf, size_t size);//在编号为fd的内容里面根据fd的offset读取size大小的内容到buf里面,可能需要底层的read,write允许跨块读,但是问题不大;
  ssize_t (*write)(int fd, const char *buf, size_t size);
  off_t (*lseek)(int fd, off_t offset, int whence);//whence有三个参数: SEEK_SET,SEEK_CUR,SEEK_END,开头,当下,最后;
  int (*mkdir)(const char *name,int index);//没想好是支持绝对路径还是相对路径,相对路径的话需要知道当前目录,需要再加一个index参数;
  int (*rmdir)(const char *name,int index);
  int (*link)(const char *name, int index);//如果是存的名字,那么可能只支持一级的link;如果name是绝对路径,我还没想好;
  int (*unlink)(const char *name);//在inode的name数组里面去掉这个name,refcnt--;
};*/
//file 在common.h的task中属于一个 属于当前进程 的 file数组flide[MAX_FILE_NUM];
struct file{
   // int exist;        //这个块活着吗？在flides里面用到;
    char name[MAX_FILE_NAME];
    int refcnt;       //表示现在开了几个,初始化为一个,在open里面++,close里面--,为0的时候从flides里面去掉;
    uint32_t vinode_index;//所处的vinode表里面的index;
    int open_offset;   //储存现在的偏移量;
 //   int fd;           //储存自己在flides数组里面的编号;
    //随用随加;
   // uint32_t mode;
    uint16_t read;
    uint16_t write;
    uint16_t rw;
};



//关于file以及flides表操作的函数;
int file_item_alloc();  //返回fd;
void file_free(int fd);//从flides里面释放相应的file_t;

//enum FS_TYPE{BLKFS=0,DEVFS=1,PROFS=2};
struct filesystem {
  char name[MAX_NAME_LENGTH];
  void*real_fs;//指向某一个操作系统的缓冲;
  //fsops_t *ops;
  dev_t *dev;
  //filesystem_t * next_filesystem;
  uint32_t type;
  void (*init)(struct filesystem *fs, const char *name, dev_t *dev);
  int (*readdir)(struct filesystem *fs, int vinode_idx, int kth, vinode_t *buf);
};

struct mount_path_point{
  int exist;
  char * name;
  char*dir_name;
  dev_t* device;//指向具体的设备;
  int index;//vinode table里面的内容;
};

  char *vfs_real_path(const char *path);
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
 int vfs_remove(const char *path);
  int vfs_create(const char *path);
  int vfs_create_file(const char *path);
  int vfs_remove_file(const char *path);
#endif