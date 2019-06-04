#include "kvdb.h"
#include <stdlib.h>
#include <stdio.h>

void process_test(){
//  kvdb_t db;
// //  const char *key = "operating-systems";
//   char *value;
//   pid_t fpid;

//   if((fpid=fork())<0){
//     perror("GG in fork!");
//     return 0;
//   }
//   else if(fpid==0){
//     const char *key="child";
//     kvdb_put(&db,key,"possiblity");
//     return 0;

//   }
//   if(kvdb(&db,""))
//   kvdb_open(&db, "process.db"); // BUG: should check for errors
//   kvdb_put(&db, key, "three-easy-pieces");
//   value = kvdb_get(&db, key);
//   kvdb_close(&db);
//   printf("[%s]: [%s]\n", key, value);
//   free(value);


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