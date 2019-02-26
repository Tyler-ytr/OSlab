#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <dirent.h>
#include <ctype.h>
#include <string.h>
//I use stat to get the information the the documents;
//I use scandir to read the catalog

/*void test()
{
    struct dirent **namelist;
    int n;
    n=scandir(".",&namelist,0,alphasort);
    if(n<0)
    {
        perror("scandir");
    }
    else
    {
        while(n--)
        {
            printf("%s/n",namelist[n]->d_name);
            free(namelist[n]);
        }
        free(namelist);
    }
}
*/
//The following function returns 1 when the folder name is a number, returns 0 otherwise.
int choose_num(const struct dirent *dir)
{
   int n=strlen(dir->d_name);
   printf("%s/n",dir->d_name);
   for(int i=0;i<n;++i)
   {
       if(!isdigit(dir->d_name[i]))
       {   printf("%c\n",dir->d_name[i]);
           return 0;
       }
       else return 1;
   }
   return 0;
}

int main(int argc, char *argv[]) {
  printf("Hello, World!\n");
  int i;
  struct dirent **namelist;
  for (i = 0; i < argc; i++) {
    assert(argv[i]); // always true
    printf("argv[%d] = %s\n", i, argv[i]);
  }
  assert(!argv[argc]); // always true
 // test();
    
  int total=scandir("/proc",&namelist,choose_num,alphasort);
  printf("total: %d",total);



  return 0;
}




//The struct of dirent
//struct dirent   
//{   
//　　long d_ino; /* inode number 索引节点号 */  
//　　   
//    off_t d_off; /* offset to this dirent 在目录文件中的偏移 */  
//    　　   
//        unsigned short d_reclen; /* length of this d_name 文件名长 */  
//        　　   
//            unsigned char d_type; /* the type of d_name 文件类型 */  
//            　　   
//                char d_name [NAME_MAX+1]; /* file name (null-terminated) 文件名，最长255字符 */  
//                }  

