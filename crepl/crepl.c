#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <signal.h>
#include <dlfcn.h>

#define MAX_LEN 512
#if defined(__i386__)
    #define ENVIRONMENT 32
#elif defined(__x86_64__)
    #define ENVIRONMENT 64
#endif

int cnt=0;

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
int check_func_valid(char * func){
    FILE *fp=NULL;
    char test_file[64];

    sprintf(test_file,"./test/test.c");
    fp=fopen(test_file,"w+");
    if(fp==NULL)
    {
        Somethingwrong("Test file GG");
    }
    fprintf(fp," #include<stdio.h>\n\nint main(){%s\nreturn 0;}\n",func);
    fclose(fp);
    int flag;

    int environment=ENVIRONMENT;
    if(environment==32)
    flag=system("gcc -m32 -shared -fPIC -Wno-implicit-function-declaration  ./test/test.c -o ./test/test.o");
    else {
    flag=system("gcc -m64 -shared -fPIC -Wno-implicit-function-declaration  ./test/test.c -o ./test/test.o");
    }

    if(flag==0)return 1;
    else return 0;


}

void *add_func_to_file(char *func,char*name)
{
    char C_file[64],SO_file[64];
    char gcc_command[256];

    sprintf(C_file,"./lib/C_%s.c",name);
    sprintf(SO_file,"./lib/SO_%s.so",name);
//    sprintf(gcc_command,"gcc -shared -fPIC -Wno-implicit-function-declaration -o %s %s",SO_file,C_file);
    int environment=ENVIRONMENT;
    if(environment==32)
    sprintf(gcc_command,"gcc -m32 -shared -fPIC -Wno-implicit-function-declaration  %s -o %s",C_file,SO_file);
    else {
    sprintf(gcc_command,"gcc -m64 -shared -fPIC -Wno-implicit-function-declaration  %s -o %s",C_file,SO_file);

    }
    //so 编码指令;
    FILE *fp=fopen(C_file,"w+");
    fprintf(fp,"%s\n",func);
    fclose(fp);

    //执行指令;
    if(system(gcc_command)!=0){
        Somethingwrong("gcc_command");
    }
    //动态加载到内存;
    return dlopen(SO_file,RTLD_GLOBAL|RTLD_LAZY);//LAZT,全局;

}

void solve_func(char *buf)
{
    char temp_name[64];

    //首先预编译康康对不对;
    // check_func;
    if(check_func_valid(buf)==0)
    {
        printf("\033[37;46mSomething wrong of your function;Please rewrite one.\033[0m\n");
        return;
    }
    //然后再加载到库里面;
    //add_func_to_file;
    sprintf(temp_name,"_expr_wrap_%04d",cnt++);
    void *flag=add_func_to_file(buf,temp_name);
/*
    void*handle=flag;
    int (*temp)();
    if(handle==NULL)assert(0);
    temp=dlsym(handle,"func");
    int result=temp();
    printf("%d\n",result);
*/
    // 对flag处理;
    if(flag!=NULL){
        printf("Add:%s\n",buf);
        return;
    }
    else{
        //printf("flag NULL!\n");
        Somethingwrong("fail to dlopen.");
    }
}

void solve_val(char *val)
{
    // to be continued
    char temp_name[64];
    char temp_val_func[64];
    sprintf(temp_name,"_expr_wrap_%04d",cnt++);
    sprintf(temp_val_func,"int %s(){return (%s);}",temp_name,val);

    //to be continued
    //检验正确性
    if(check_func_valid(temp_val_func)==0)
    {
        printf("\033[37;46mSomething wrong of your expression;Please rewrite one.\033[0m\n");
        return;
    }

    void * handle=add_func_to_file(temp_val_func,temp_name);

    int (*temp)();
    if(handle==NULL)return;
   temp=dlsym(handle,temp_name);
   if(temp!=NULL)
   {
       printf("%s = %d\n",val,temp());
   }
   else{
       Somethingwrong("dlsym return NULL");
   }

return;
}



int main(int argc, char *argv[]) {
    //创建测试目录以及lib目录;
    printf("使用exit()退出;如果使用了Ctrl+C 退出,需要在经历一次报错之后重新跑;\n");
    printf("允许一定的错误(比如1++1),但是使用未定义的函数名会导致程序终止,如果这样的话,同样需要先执行一次经历一次报错之后才能重新跑\n");

    if(system("mkdir test")!=0)
        Somethingwrong("mkdir test");
    if(system("mkdir lib")!=0)
        Somethingwrong("mkdir lib");
    //输入部分;
    char in[MAX_LEN];
    while(my_read(">>",in)!=NULL)
    {
        if(in==NULL)continue;
        if(strcmp(in,"exit()")==0)
            break;
        //判断是函数还是表达式;
        if(strncmp("int ",in,4)==0)
        {
            //是func
     //   printf("Func:%s\n",in);
            solve_func(in);

        }else
        {
            //是表达式;
       // printf("Val:%s\n",in);
            solve_val(in);
        }
    }

    Exitcrepl();
//    printf("Success");

/*
    char temp_name[50];
   char SO_file[50];
   sprintf(temp_name,"_expr_wrap_%04d",0);

    sprintf(SO_file,"./lib/SO_%s.so",temp_name);
    printf("%s\n,",SO_file);
   if( dlopen(SO_file,RTLD_GLOBAL|RTLD_NOW)==NULL)printf("fail");else printf("success");//LAZT,全局;
    void*handle=dlopen(SO_file,RTLD_GLOBAL|RTLD_NOW);
   int (*temp)();
    if(handle==NULL)assert(0);
   temp=dlsym(handle,"func");
    int result=temp();
    printf("%d\n",result);
*/
    return 0;
}

//参考:
//分析dl系列:https://blog.csdn.net/zhengqijun_/article/details/72540878
