#ifndef _VFS_H
#define _VFS_H

typedef struct fsops fsops_t;
typedef struct indeops indeops_t;
typedef struct inode inode_t;
typedef struct filesystem fs_t;

struct inode {
  
  int refcnt;      //次数;
  void *ptr;       // private data
  filesystem_t *fs;
  inodeops_t *ops; // 在inode被创建时，由文件系统的实现赋值
                   // inode ops也是文件系统的一部分
  
};
struct inodeops {
  int (*open)(file_t *file, int flags);
  int (*close)(file_t *file);
  ssize_t (*read)(file_t *file, char *buf, size_t size);
  ssize_t (*write)(file_t *file, const char *buf, size_t size);
  off_t (*lseek)(file_t *file, off_t offset, int whence);
  int (*mkdir)(const char *name);
  int (*rmdir)(const char *name);
  int (*link)(const char *name, inode_t *inode);
  int (*unlink)(const char *name);
  // 你可以自己设计readdir的功能
};

struct filesystem {
  void*fs;
  fsops_t *ops;
  dev_t *dev;
};
struct fsops {
  void (*init)(struct filesystem *fs, const char *name, dev_t *dev);
  inode_t *(*lookup)(struct filesystem *fs, const char *path, int flags);
  int (*close)(inode_t *inode);
};



#endif