// #include "kvdb.h"
// #include <stdlib.h>
// #include <stdio.h>
// #include <unistd.h>

// #define Log2(format,...)\
//  do { \
//     printf("\33[1;34m" format "\33[0m\n" \
//         ); \
//   } while (0)//蓝色;
// #define Log3(format,...)\
//  do { \
//     printf("\33[1;31m format \33[0m\n" \
//         ); \
//   } while (0)//红色;
// void process_test(){
// //  const char *key = "operating-systems";
//   char *value;
//   pid_t fpid;

//   if((fpid=fork())<0){
//     Log3(GG in fork!);
//     return ;
//   }
//   else if(fpid==0){
//  kvdb_t db;
//     kvdb_open(&db, "process.db"); 
//     const char *keyc="child";
//     kvdb_put(&db,keyc,"possiblity");
//     kvdb_close(&db);
//     kvdb_open(&db, "process.db"); 
//     int cnt=0;
//     while(1){
//       if(cnt==50)break;
//       char str[3];
//       char key[8];
//      // str[0]=cnt%100+'0';
//       str[0]=cnt/10+'0';
//       str[1]=cnt%10+'0';
//       str[2]='\0';
//       key[0]='c';
//       key[1]='h';
//       key[2]='i';
//       key[3]='l';
//       key[4]='d';
//       key[5]=str[0];
//       key[6]=str[1];
//       key[7]=str[2];
//      // printf("%s\n",str);
//       if(kvdb_put(&db,key,str)!=0){
//         Log3("GG in child put process!");
//         return ;
//       }
//       cnt++;
//     }
//     char *valuea;
//     cnt=0;
//     while(1){
//       if(cnt==50)break;
//       char str[3];
//       char key[8];
//      // str[0]=cnt%100+'0';
//       str[0]=cnt/10+'0';
//       str[1]=cnt%10+'0';
//       str[2]='\0';
//       key[0]='c';
//       key[1]='h';
//       key[2]='i';
//       key[3]='l';
//       key[4]='d';
//       key[5]=str[0];
//       key[6]=str[1];
//       key[7]=str[2];
//      // printf("%s\n",str);
//      valuea = kvdb_get(&db, key);
//      if(valuea==(void *)-1){
//        Log3("GG in child get process!");
//        return ;
//      }
//      if(cnt==29){
//       printf("[%s]: [%s]\n", key, valuea);
//      }
//      free(valuea);
//       cnt++;
//     }
//     kvdb_close(&db);

//   //  return ;

//   }else{
//     kvdb_t db;
//     kvdb_open(&db, "process.db"); 
//     int cnt=0;
//     while(1){
//       if(cnt==50)break;
//     const char *key="father";
//     kvdb_put(&db,key,"ability");
//     cnt++;
//     }
//     kvdb_close(&db);
//   }

//  kvdb_t db;
//   const char *key1="child30";
//   const char *key2="father";
//   char *value1;
//    kvdb_open(&db, "process.db"); // BUG: should check for errors
//   value1 = kvdb_get(&db, key1);
//   kvdb_close(&db);
//   printf("[%s]: [%s]\n", key1, value1);
//   free(value1);
  


// //   // if(kvdb(&db,""))
// //   // kvdb_open(&db, "process.db"); // BUG: should check for errors
// //   // kvdb_put(&db, key, "three-easy-pieces");
// //   // value = kvdb_get(&db, key);
// //   // kvdb_close(&db);
// //   // printf("[%s]: [%s]\n", key, value);
// //   // free(value);


// ;
// }
// void thread_test(){
  
//   ;
// }

// int main() {
//   kvdb_t db;
//   const char *key = "operating-systems";
//   char *value;

//   kvdb_open(&db, "a.db"); // BUG: should check for errors
//   kvdb_put(&db, key, "three-easy-pieces");
//   value = kvdb_get(&db, key);
//   kvdb_close(&db);
//   printf("[%s]: [%s]\n", key, value);
//   free(value);

//   process_test();
//   thread_test();
 

//   return 0;
// }

#include "kvdb.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <stdint.h>
#include <assert.h>

#define NUMTHREADS 100
#define TESTNUM 100
#define FILESIZE 1000

#define handle_error(msg) \
    do {perror(msg); exit(EXIT_FAILURE); } while (0)

kvdb_t db;
void * test1(void *data){
    char *key = (char *)malloc(sizeof(char)*FILESIZE);
    char *value;

    /*kvdb_t db;*/
    kvdb_open(&db, "a.db"); // BUG: should /heck for errors
    /*kvdb_put(&db, key, "three-easy-pieces");*/
    char *buf = malloc(sizeof(char)*FILESIZE);
    if (buf == NULL) handle_error("malloc failure\n");
    for(int i = 0; i < TESTNUM; i++){
        sprintf(buf, "operating-%d-systems", i);
        sprintf(key, "operating-%d-sys-hello", i);
        kvdb_put(&db, buf, key);
    }

    for(int i = 0; i < TESTNUM; i++){
        sprintf(buf, "operating-%d-systems", i);
        sprintf(key, "operating-%d-sys-hello-world", i);
        if((i  % 2) == 0)
            kvdb_put(&db, buf, key);
        else{
            sprintf(key, "operating-%d-sys", i);
            kvdb_put(&db, buf, key);
        }
    }
    for(int i = 0; i < TESTNUM; i++){
        sprintf(buf, "operating-%d-systems", i);
        value = kvdb_get(&db, buf);
        printf("[%s]: [%s]\n", buf, value);
        free(value);
    }
    free(key);
    free(buf);
    kvdb_close(&db);
}

void* thread_test(void * data){
    int chret;
    chret = kvdb_open(&db, "a.db");
    /*chret = kvdb_open(&dc, "c.db");*/
    if(chret < 0) handle_error("open file failed in tests\n");
    char *key = (char *)malloc(sizeof(char)*FILESIZE);
    char *buf = (char *)malloc(sizeof(char)*FILESIZE);
    char *value;
    uintptr_t no = (uintptr_t)data;
    for(int i = 0; i < TESTNUM; i++){
        sprintf(key, "operating-%d-system", i);
        sprintf(buf, "operating-%d-system", i);
        kvdb_put(&db, key, buf);
        /*kvdb_put(&dc, key, buf);*/
    }
    for(int i = 0; i < TESTNUM; i++){
        if((i % 2) == 0){
            sprintf(key, "operating-%d-system", i);
            sprintf(buf, "operating-%d-system-hello", i);
            kvdb_put(&db, key, buf);
            /*kvdb_put(&dc, key, buf);*/
        }
    }
    for(int i = 0; i < TESTNUM; i++){
        sprintf(key, "operating-%d-system", i);
        /*if((i % 2) == 0 )*/
            value = kvdb_get(&db, key);
        /*else*/
            /*value = kvdb_get(&dc, key);*/
        printf("[%s]: [%s]: [thread-%ld]\n", key, value, (unsigned long)no);
        free(value);
    }
    free(key);
    free(buf);
}

int pthread_test(){
    int rc;
    uintptr_t t;
    pthread_t thread[NUMTHREADS];
    for(t = 0; t < NUMTHREADS; t++){
        printf("Creating Thread %ld\n", (unsigned long)t + 1);
        rc = pthread_create(&thread[t], NULL, thread_test, (void *)(t));
        if(rc){
            printf("ERROR, return code is %d\n", rc);
            handle_error("ERROR\n");
        }
    }
    for(t = 0; t < NUMTHREADS; t++){
        pthread_join(thread[t], NULL);
    }
    /*assert(0);*/
    int err = kvdb_close(&db);
    if(err == -1){
        handle_error("close file failed in tests\n");
    }
}

int main(){
    /*test1(100);*/

    // pid_t pid = fork();
    // if(pid == 0){
    //     pthread_test();
    // }else{
    //     pid_t ppid = fork();
    //     if(ppid == 0)
    //         pthread_test();
    //     else
    //         pthread_test();
    // }

    //pthread_test(1);

    kvdb_open(&db, "a.db");
    const char *key = "operating-systems";
    kvdb_put(&db, key, "three-easy-pieces");
    kvdb_put(&db, key, "three-easy-pieces-pk");
    kvdb_put(&db, key, "three-easy");
    kvdb_put(&db, "helloworld", "three-easy-pieces");
    char *value = kvdb_get(&db, key);
    printf("[%s]:[%s]\n", key, value);
    free(value);
    kvdb_close(&db);
    return 0;
}
