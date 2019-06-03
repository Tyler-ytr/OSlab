#ifndef __KVDB_H__
#define __KVDB_H__


struct kvdb {
};
typedef struct kvdb kvdb_t;

int kvdb_open(kvdb_t *db, const char *filename);
//kvdb_open打开filename数据库文件(例如filename指向"a.db")，并将信息保存到db中。如果文件不存在，则创建，如果文件存在，则在已有数据库的基础上进行操作。
int kvdb_close(kvdb_t *db);
//kvdb_close关闭数据库并释放相关资源。关闭后的kvdb_t将不再能执行put/get操作；但不影响其他打开的kvdb_t。
int kvdb_put(kvdb_t *db, const char *key, const char *value);
//kvdb_put建立key到value的映射，如果把db看成是一个std::map<std::string,std::string>，则相当于执行db[key] = value;。因此如果在kvdb_put执行之前db[key]已经有一个对应的字符串，它将被value覆盖。
char *kvdb_get(kvdb_t *db, const char *key);
//kvdb_get获取key对应的value，相当于返回db[key]。返回的value是通过动态内存分配实现的(例如malloc或strdup分配的空间)，因此在使用完毕后需要调用free释放。
//如果db不合法、内存分配失败或key不存在，则kvdb_get返回空指针。
#endif
