#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#define MAX_LEN 512

void Exitcrepl();
void Somethingwrong(char *str)
{
    perror(str);
    Exitcrepl();
    exit(EXIT_FAILURE);
}

void Exitcrepl()
{
    if(system("rm -rf test")!=0)
    {
        perror("rm -rf test");
        assert(0);
    }
    if(system("rm -rf lib")!=0)
    {
        perror("rm -rf lib");
        assert(0);
    }

}

char* my_read(const char *previous,char * buf){
    printf("%s ",previous);
    char *result=NULL;
    result=fgets(buf,MAX_LEN,stdin);
    if(ferror(stdin)&&result==NULL)
    {
        Somethingwrong("my_read");
    }
    int len=strlen(buf);
    buf[len-1]='\0';
    return result;

}

int main(int argc, char *argv[]) {
    //创建测试目录以及lib目录;
    printf("使用exit()退出;如果使用了Ctrl+C 退出,需要在经历一次报错之后重新跑;\n");
    if(system("mkdir test")!=0)
        Somethingwrong("mkdir test");
    if(system("mkdir lib")!=0)
        Somethingwrong("mkdir lib");
    //输入部分;
    char in[MAX_LEN];
    while(my_read(">>",in)!=NULL)
    {
        if(strcmp(in,"exit()")==0)
            break;
        //printf("%s\n",in);

    }

    Exitcrepl();
    printf("Success");


    return 0;
}
