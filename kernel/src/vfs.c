#include <blkfs.h>
#include <vfs.h>


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


void vfs_init(){



  ;
}

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




