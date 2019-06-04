#include "kvdb.h"
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/file.h>
#include <pthread.h>
#include <assert.h>
//static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;


//pthread锁的使用参考了:https://feng-qi.github.io/2017/05/08/pthread-mutex-basic-usage/

//fcntl来实现文件锁参考了:https://www.cnblogs.com/mingfeng002/p/6962915.html

//文件锁的实现:

// static void set_readlock(pid_t fd){
//   struct flock lock;
//   //以下的三个参数用于分段对文件加锁，若对整个文件加锁，则：l_whence=SEEK_SET, l_start=0, l_len=0
//   lock.l_whence=SEEK_SET;//决定l_start位置；
//   lock.l_start=0;//锁定区域开头位置
//   lock.l_len=0;//锁定区域大小; 
//   lock.l_type=F_RDLCK;//锁定的状态; 现在表示读取锁;
//   lock.l_pid=getpid();//锁定动作的进程；
  
//   fcntl(fd, F_SETLK, &lock);//F_SETLK:给文件上锁的参数;

// }
// static void set_writelock(pid_t fd){
//   struct flock lock;
//   //以下的三个参数用于分段对文件加锁，若对整个文件加锁，则：l_whence=SEEK_SET, l_start=0, l_len=0
//   lock.l_whence=SEEK_SET;//决定l_start位置；
//   lock.l_start=0;//锁定区域开头位置
//   lock.l_len=0;//锁定区域大小; 
//   lock.l_type=F_WRLCK;//锁定的状态; 现在表示写入锁;
//   lock.l_pid=getpid();//锁定动作的进程；
  
//   fcntl(fd, F_SETLKW, &lock);//F_SETLK:给文件上锁的参数;

// }
// static void file_unlock(pid_t fd){
//   struct flock lock;
//   //以下的三个参数用于分段对文件加锁，若对整个文件加锁，则：l_whence=SEEK_SET, l_start=0, l_len=0
//   lock.l_whence=SEEK_SET;//决定l_start位置；
//   lock.l_start=0;//锁定区域开头位置
//   lock.l_len=0;//锁定区域大小; 
//   lock.l_type=F_UNLCK;//锁定的状态; 现在表示解锁;
//   lock.l_pid=getpid();//锁定动作的进程；
  
//   fcntl(fd, F_SETLKW, &lock);//F_SETLK:给文件上锁的参数;

// }
static void level1_error(const char *error){
  perror(error);
}



//kvdb_open打开filename数据库文件(例如filename指向"a.db")，并将信息保存到db中。如果文件不存在，则创建，如果文件存在，则在已有数据库的基础上进行操作。
int kvdb_open_origin(kvdb_t *db, const char *filename)
{//绝对不安全,请记得修改;
  strcpy(db->name,filename);
  if((db->fp=fopen(filename,"a+"))==NULL){
    level1_error("fopen");
    
    return -1;
  };

  return 0;
}


//kvdb_close关闭数据库并释放相关资源。关闭后的kvdb_t将不再能执行put/get操作；但不影响其他打开的kvdb_t。
int kvdb_close_origin(kvdb_t *db){
  fclose(db->fp);
  return 0;
}
//kvdb_put建立key到value的映射，如果把db看成是一个std::map<std::string,std::string>，则相当于执行db[key] = value;。因此如果在kvdb_put执行之前db[key]已经有一个对应的字符串，它将被value覆盖。
int kvdb_put_origin(kvdb_t *db, const char *key, const char *value){
  fseek(db->fp,0,SEEK_END);//读写位置移动到文件尾
  fwrite(key,1,strlen(key),db->fp);
  fwrite("\n",1,1,db->fp);
  fwrite(value,1,strlen(value),db->fp);
  fwrite("\n",1,1,db->fp);
  return 0;

}
//kvdb_get获取key对应的value，相当于返回db[key]。返回的value是通过动态内存分配实现的(例如malloc或strdup分配的空间)，因此在使用完毕后需要调用free释放。
//如果db不合法、内存分配失败或key不存在，则kvdb_get返回空指针。
char *kvdb_get_origin(kvdb_t *db, const char *key){
  //按照上面的不安全的存储,现在是 key,\n,value,\n;
  char *result=NULL;
  static char _key[1<<20],_value[1<<20];
  fseek(db->fp,0,SEEK_SET);//将读写位置移动到文件的开头;

  while(1){
    //一行一行往下面读;
    if(!fgets(_key,sizeof(_key),db->fp))break;
    if(!fgets(_value,sizeof(_value),db->fp))break;
    _key[strlen(_key) - 1] = '\0';
    _value[strlen(_value) - 1] = '\0';
    if (strcmp(key, _key) == 0) {
      if (!result) free(result);
      result = strdup(_value);
    }
    
  }
  return result;

}


int kvdb_open(kvdb_t *db, const char *filename){
  
  if(db->inited!=1984){pthread_mutex_init(&db->mutex_lock,NULL);
  db->inited=1984;
  }

  if(pthread_mutex_lock(&db->mutex_lock)!=0){
  //if(pthread_mutex_lock(&mutex)!=0){
    perror("error: mutex_lock in kvdb_open");
    return -1;
  }
  int result=kvdb_open_origin(db,filename);
  if(pthread_mutex_unlock(&db->mutex_lock)!=0){
  //if(pthread_mutex_unlock(&mutex)!=0){
    perror("error: mutex_unlock in kvdb_open");
    return -1;
  }
  return result;
}

int kvdb_close(kvdb_t *db){
  if(pthread_mutex_lock(&db->mutex_lock)!=0){
  //if(pthread_mutex_lock(&mutex)!=0){
    perror("error: mutex_lock in kvdb_close");
    return -1;
  }
  int result=kvdb_close_origin(db);
  if(pthread_mutex_unlock(&db->mutex_lock)!=0){
//  if(pthread_mutex_unlock(&mutex)!=0){
    perror("error: mutex_unlock in kvdb_close");
    return -1;
  }
  pthread_mutex_destroy(&db->mutex_lock);
  
  return result;
}


char *kvdb_get(kvdb_t *db, const char *key){
if(pthread_mutex_lock(&db->mutex_lock)!=0){
//if(pthread_mutex_lock(&mutex)!=0){
  perror("error: mutex_lock in kvdb_get");
  return NULL;
}

char *result=kvdb_get_origin(db,key);
if(pthread_mutex_unlock(&db->mutex_lock)!=0){
//if(pthread_mutex_unlock(&mutex)!=0){
  perror("error: mutex_unlock in kvdb_get");
  return NULL;
}
  return result;
}

int kvdb_put(kvdb_t *db, const char *key, const char *value){
  if(pthread_mutex_lock(&db->mutex_lock)!=0){
 // if(pthread_mutex_lock(&mutex)!=0){
    perror("error: mutex_lock in kvdb_put");
    return -1;
  }
  int result=kvdb_put_origin(db,key,value);
  if(pthread_mutex_unlock(&db->mutex_lock)!=0){
  //if(pthread_mutex_unlock(&mutex)!=0){
    perror("error: mutex_unlock in kvdb_put");
    return -1;
  }
return result;

}

