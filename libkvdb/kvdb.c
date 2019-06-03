#include "kvdb.h"
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/file.h>
#include <pthread.h>
#include <assert.h>
//pthread锁的使用参考了:https://feng-qi.github.io/2017/05/08/pthread-mutex-basic-usage/



//kvdb_open打开filename数据库文件(例如filename指向"a.db")，并将信息保存到db中。如果文件不存在，则创建，如果文件存在，则在已有数据库的基础上进行操作。
int kvdb_open(kvdb_t *db, const char *filename)
{//绝对不安全,请记得修改;
  db->fp=fopen(filename,"a+");

  return 0;
}

//kvdb_close关闭数据库并释放相关资源。关闭后的kvdb_t将不再能执行put/get操作；但不影响其他打开的kvdb_t。
int kvdb_close(kvdb_t *db){
  fclose(db->fp);
  return 0;
}
//kvdb_put建立key到value的映射，如果把db看成是一个std::map<std::string,std::string>，则相当于执行db[key] = value;。因此如果在kvdb_put执行之前db[key]已经有一个对应的字符串，它将被value覆盖。
int kvdb_put(kvdb_t *db, const char *key, const char *value){
  fseek(db->fp,0,SEEK_END);//读写位置移动到文件尾
  fwrite(key,1,strlen(key),db->fp);
  fwrite("\n",1,1,db->fp);
  fwrite(value,1,strlen(value),db->fp);
  fwrite("\n",1,1,db->fp);
  return 0;

}
//kvdb_get获取key对应的value，相当于返回db[key]。返回的value是通过动态内存分配实现的(例如malloc或strdup分配的空间)，因此在使用完毕后需要调用free释放。
//如果db不合法、内存分配失败或key不存在，则kvdb_get返回空指针。
char *kvdb_get(kvdb_t *db, const char *key){
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

}









