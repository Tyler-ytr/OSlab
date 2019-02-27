#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <dirent.h>
#include <ctype.h>
#include <string.h>
//I use stat to get the information the the documents;
//I use scandir to read the catalog
//https://blog.csdn.net/lk07828/article/details/52032479 A website which shows the structure of the /proc/[pid]/status
//http://man7.org/linux/man-pages/man5/proc.5.html A good manual online of procfs
typedef struct proc_status
{
  char name[200];   //Name
  int pid;          //The process ID;
  int ppid;         //PID of parent process.
  int vis;          //I wonder if it has been visited;The original of it is 0, it will be 1 if visited;
  int depth;        //The depth of the node;
  //int child_pid[1000];
  //int child_pid_number;
}status;

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
   //printf("%s\n",dir->d_name);
   for(int i=0;i<n;++i)
   {
       if(!isdigit(dir->d_name[i]))

           return 0;
       else return 1;
   }
   return 0;
}
extern int check_name(char *str);
extern int get_pid(char *str);
extern int get_ppid(char *str);

void proc_print(status *proc,int total,int ppid,int depth,int flag)
{
    int i=0;
    int j=0;
    int t=0;
    for(i=0;i<total;i++)
    {
        if(proc[i].ppid==ppid&&proc[i].vis==0)
        {
            proc[i].depth=depth+1;
            proc[i].vis=1;
            for(j=0;j<depth;j++)
            {
                printf("        ");
            }
            if(proc[i].pid>0)
            {
                printf("t: %d ",t);
                printf("|----%s(%d)",proc[i].name,proc[i].pid);
                if(flag==1)
                {
                    printf("\n");
                }
            }
                if(t==0)
                {
                    proc_print(proc,total,proc[i].pid,proc[i].depth,0);
                }else
                {
                    proc_print(proc,total,proc[i].pid,proc[i].depth,1);

                }
                    t++;
        }
    }
}
int main(int argc, char *argv[]) {
  printf("Hello, World!\n");
  
  int i;
  int proc_t=0;
  struct dirent **namelist;
  status proc[20480];
  char temp_proc_path[100];
  char str[1025];
  char name[200];
  const char charproc[7]="/proc/";
  //printf("%s\n\n",charproc);
 //int len=strlen("/proc");
  //printf("%d\n\n",len);
  FILE *fp;
  
  for (i = 0; i < argc; i++) {
    assert(argv[i]); // always true
    printf("argv[%d] = %s\n", i, argv[i]);
    if(strncmp(argv[i],"-v",2)==0)
    {
        printf("version: 1.0\n");
    }
  }
  assert(!argv[argc]); // always true
 // test();
    
  int total=scandir("/proc",&namelist,choose_num,alphasort);
  printf("total: %d\n",total);
  if(total<0)
  {
    printf("GG of total! There are wrong in scandir!!");
  }

  
  for(int i=0;i<total;++i)
  {
    //printf("%s\n",namelist[i]->d_name);
    strcpy(temp_proc_path,charproc);
    strcat(temp_proc_path,namelist[i]->d_name);
    strcat(temp_proc_path,"/status");
    //printf("%s\n",temp_proc_path);
    //temp_proc_path : "/proc/[pid]/status"
    
    fp=fopen(temp_proc_path,"r");
    while(1)
    {
        fgets(str,1024,fp);
        if(feof(fp))
        {
            break;
        }
    
        //printf("%s",str);
        int temp_len=strlen(str);
        //printf("len: %d\n",temp_len);
        if(check_name(str)!=-1)
        {
            int k=check_name(str);
            int cnt;cnt=0;
            for(int i=k;i<temp_len;i++)
            { 
                name[cnt]=str[i];
                cnt++;
            }
            name[cnt-1]='\0';
            strcpy(proc[proc_t].name,name);
        }

        int temp_pid;
        if(get_pid(str)!=-1)
        {
            temp_pid=get_pid(str);
            //printf("pidpidpid%d\n\n\n\n",temp_pid);
            //break;
            proc[proc_t].pid=temp_pid;
        }
       
        int temp_ppid;
        if(get_ppid(str)!=-1)
        {
            temp_ppid=get_ppid(str);
            //printf("pidpidpid%d\n\n\n\n",temp_ppid);
            proc[proc_t].ppid=temp_ppid;
            break;
        }



    }
    fclose(fp);
            printf("proc_t: %d ",proc_t);
            printf("name: %s ",proc[proc_t].name);
            printf("pid: %d ",proc[proc_t].pid);
            printf("ppid: %d\n\n",proc[proc_t].ppid);
    proc_t++;
  }
    //memset(&proc->ppid_num,-1,total);
    memset(&proc->vis,0,total);
    memset(&proc->depth,0,total);
   // test_print(proc,total,0,0);
    proc_print(proc,total,0,0,0);
  //int flag=1;
  return 0;
}

int check_name(char *str)
{
    int temp_len=strlen(str);
    if(strncmp(str,"Name",4)!=0)
        return -1;
    else 
    {   int i;
        for(i=4;i<temp_len;++i)
        {
            if(str[i]>='a'&&str[i]<='z')
            {
                break;
            }
        }
        return i;

    }
}
int get_pid(char *str)
{   
    int cnt=0;
    char number[100];
    int temp_len=strlen(str);
    if(strncmp(str,"Pid",3)!=0)
    {
        return -1;
    }
    else 
    {
        int t;
        for(t=0;t<temp_len;++t)
        {
            if(str[t]>='0'&&str[t]<='9')
            {
                break;
            }
        }
        for(int i=t;i<temp_len;++i)
        {
            number[cnt]=str[i];
            cnt++;
        }
    }
    int num=atoi(number);
    return num;

}

int get_ppid(char *str)
{
    int cnt=0;
    char number[100];
    int temp_len=strlen(str);
    if(strncmp(str,"PPid",4)!=0)
    {
        return -1;
    }
    else 
    {
        int t;
        for(t=0;t<temp_len;++t)
        {
            if(str[t]>='0'&&str[t]<='9')
            {
                break;
            }
        }
        for(int i=t;i<temp_len;++i)
        {
            number[cnt]=str[i];
            cnt++;
        }
    }
    int num=atoi(number);
    return num;
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

