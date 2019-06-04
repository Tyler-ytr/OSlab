#include "kvdb.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#define Log2(format,...)\
 do { \
    printf("\33[1;34m[%s,%d,%s] " format "\33[0m\n", \
        __FILE__, __LINE__, __func__, ## __VA_ARGS__); \
  } while (0)//蓝色;
#define Log3(format,...)\
 do { \
    printf("\33[1;33m[%s,%d,%s] " format "\33[0m\n", \
        __FILE__, __LINE__, __func__, ## __VA_ARGS__); \
  } while (0)//红色;
void process_test(){
//  const char *key = "operating-systems";
  char *value;
  pid_t fpid;

  if((fpid=fork())<0){
    Log3("GG in fork!");
    return ;
  }
  else if(fpid==0){
 kvdb_t db;
    kvdb_open(&db, "process.db"); 
    const char *keyc="child";
    kvdb_put(&db,keyc,"possiblity");
    kvdb_close(&db);
    kvdb_open(&db, "process.db"); 
    int cnt=0;
    while(1){
      if(cnt==50)break;
      char str[3];
      char key[8];
     // str[0]=cnt%100+'0';
      str[0]=cnt/10+'0';
      str[1]=cnt%10+'0';
      str[2]='\0';
      key[0]='c';
      key[1]='h';
      key[2]='i';
      key[3]='l';
      key[4]='d';
      key[5]=str[0];
      key[6]=str[1];
      key[7]=str[2];
     // printf("%s\n",str);
      if(kvdb_put(&db,key,str)!=0){
        Log3("GG in child put process!");
        return ;
      }
      cnt++;
    }
    char *value;
    while(1){
      if(cnt==50)break;
      char str[3];
      char key[8];
     // str[0]=cnt%100+'0';
      str[0]=cnt/10+'0';
      str[1]=cnt%10+'0';
      str[2]='\0';
      key[0]='c';
      key[1]='h';
      key[2]='i';
      key[3]='l';
      key[4]='d';
      key[5]=str[0];
      key[6]=str[1];
      key[7]=str[2];
     // printf("%s\n",str);
     value = kvdb_get(&db, key);
     if(value==(void *)-1){
       Log3("GG in child get process!");
       return ;
     }
     free(value);
     if(cnt==30){
      printf("[%s]: [%s]\n", key, value);
     }
      cnt++;
    }
    kvdb_close(&db);

    return ;

  }else{
 kvdb_t db;
    kvdb_open(&db, "process.db"); 
    int cnt=0;
    while(1){
      if(cnt==50)break;
    const char *key="father";
    kvdb_put(&db,key,"ability");
    cnt++;
    }
    kvdb_close(&db);
  }

 kvdb_t db;
  const char *key1="child30";
  const char *key2="father";
  char *value1;
   kvdb_open(&db, "a.db"); // BUG: should check for errors
  value1 = kvdb_get(&db, key2);
  kvdb_close(&db);
  printf("[%s]: [%s]\n", key2, value1);
  free(value1);
  


//   // if(kvdb(&db,""))
//   // kvdb_open(&db, "process.db"); // BUG: should check for errors
//   // kvdb_put(&db, key, "three-easy-pieces");
//   // value = kvdb_get(&db, key);
//   // kvdb_close(&db);
//   // printf("[%s]: [%s]\n", key, value);
//   // free(value);


;
}
void thread_test(){
  
  ;
}

int main() {
  kvdb_t db;
  const char *key = "operating-systems";
  char *value;

  kvdb_open(&db, "a.db"); // BUG: should check for errors
  kvdb_put(&db, key, "three-easy-pieces");
  value = kvdb_get(&db, key);
  kvdb_close(&db);
  printf("[%s]: [%s]\n", key, value);
  free(value);

  process_test();
  thread_test();
 


  return 0;
}