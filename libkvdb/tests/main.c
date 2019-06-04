#include "kvdb.h"
#include <stdlib.h>
#include <stdio.h>

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
//  kvdb_t db;
// //  const char *key = "operating-systems";
//   char *value;
//   pid_t fpid;

//   if((fpid=fork())<0){
//     Log3("GG in fork!");
//     return 0;
//   }
//   else if(fpid==0){
//     kvdb_open(&db, "process.db"); 
//     const char *key="child";
//     kvdb_put(&db,key,"possiblity");
//     int cnt=0;
//     while(1){
//       if(cnt==500)break;
//       char str[3];
//       str[0]=cnt%100+'0';
//       str[1]=cnt%10+'0';
//       str[2]='\0';

//       if(kvdb_put(&db,"child",str)!=0){
//         Log3("GG in child process!");
//         return 0;
//       }
//       cnt++;
//     }

//     return 0;

//   }else{
//     kvdb_open(&db, "process.db"); 
//     const char *key="father";
//     kvdb_put(&db,key,"ability");
//   }

//   const char *key1="child";
//   const char *key2="father";
  


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
    int cnt=0;
    while(1){
      if(cnt==50)break;
      char str[2];
     // str[0]=cnt%100+'0';
      str[1]=cnt%10+'0';
      str[2]='\0';
      printf("%s\n",str);
      if(kvdb_put(&db,"child",str)!=0){
        Log3("GG in child process!");
        return 0;
      }
      cnt++;
    }



  return 0;
}