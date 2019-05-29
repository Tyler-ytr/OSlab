#include <sys/mman.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <fcntl.h>
#include<stdlib.h>
#include <sys/stat.h> 
#include <stdint.h>
#include<string.h>
 typedef struct Mbr{
  uint8_t jmp_code[3];//跳转指令;
  uint8_t OEM[8];//OEM串;
  uint8_t sec_bit_num[2];//每一个扇区的字节数
  uint8_t cluster_sec_num[1];//每簇的扇区数;
  uint8_t res_sec_num[2];//保留的扇区数;
  uint8_t fat_num[1];//FAT表的个数;
  uint8_t dir_num[2];//根目录最多容纳的目录项数
  uint8_t tot_sec[2];//扇区总数;
  uint8_t medium[1];//介质描述;
  uint8_t fat_sec_num0[2];//每一个fat表的扇区数;FAT32不使用,为0
  uint8_t track_sec_num[2];//每一个磁道的扇区数;
  uint8_t mag_num[2];//磁头数; 
  uint8_t sec_stat[4];//分区前的扇区状态;
  uint8_t fs_sec_num[4];//文件系统扇区数量;
  uint8_t fat_sec_num[4];//每一个FAT表的扇区数量;
  uint8_t mark[2];//标记
  uint8_t version[2];//版本号
  uint8_t root_cluster[4];//根目录簇号;
  uint8_t fsinfo_cluster[2];//FSINFO扇区号;
  uint8_t backup_address[2];//备份引导扇区的位置;
  uint8_t useless1[12];
  uint8_t bios_device[1];//bios设备号码;
  uint8_t useless2[1];
  uint8_t extend_mark[1];//扩展引导标志;
  uint8_t vol_num[4];//卷序列号;
  uint8_t vol_ascii[11];//卷标(ASCII);
  uint8_t fs_ascii[8];//文件系统的ASCII码;
  uint8_t useless3[420];
  uint8_t end[2];//结束标志;
 }MBR;
typedef struct My_mbr{
  uint16_t sec_bit_num;     //每一个扇区的字节数
  uint8_t cluster_sec_num;  //每簇的扇区数;
  uint16_t res_sec_num;     //保留的扇区数;
  uint32_t fat_sec_num;//每一个FAT表的扇区数量; 
  uint32_t root_cluster;//根目录簇号;
  //根目录起始扇区:N=保留区大小+2*FAT表大小;
  void * root_address;//起始扇区的位置=start+N*扇区大小;
  int difference;
}MY_MBR;

typedef struct Directory_short_item{
  uint8_t file_name[8];//文件名;
  uint8_t extend_name[3];//文件扩展名;
  uint8_t state[1];//文件的属性字节 0 读写 01 只读 10 隐藏 100系统 1000卷标 10000子目录 100000归档
  uint8_t reserved[1];//系统保留;
  uint8_t create_10m_time[1];//创建时间的10毫秒位;
  uint8_t create_time[2];//文件创建时间;
  uint8_t create_day[2];//文件创建日期;
  uint8_t vis_day[2];//文件最后访问日期;
  uint8_t h_b_cluster[2];//文件起始簇号的高16位
  uint8_t modify_time[2];//文件最后修改时间;
  uint8_t modify_day[2];//文件最后修改日期;
  uint8_t l_b_cluster[2];//文件起始簇号的低16位;
  uint8_t length[4];//表示文件的长度;
}dir_s_item;

typedef struct My_dir_s_item{
  char file_name[9];//文件名;
  char extend_name[8];//文件扩展名;`
  uint32_t located_cluster;//文件所在的簇;
  void *address;
  uint32_t length;

}my_dir_s_item;
//my_dir_s_item short_list[1000];


typedef struct Directory_long_item{
  uint8_t state[1];//0-4字节顺序取值,5保留,第6个字节表示是长文件最后一个目录项;
  uint8_t name1[10];//长文件名unicode码1
  uint8_t attr[1];//文件属性,只能是0x0f;
  uint8_t reserved[1];//系统保留;
  uint8_t checked[1];//校验值;根据短文件名验证得到;
  uint8_t name2[12];//长文件名unicode码2；
  uint8_t b_cluster[2];//文件起始簇号;
  uint8_t name3[4];//长文件名unicode码3;
}dir_l_item;

typedef struct Row{
  uint8_t bit[16];//0xB：描述文件的属性，该字段在短文件中不可取值0x0F，如果设置为0x0F则标志是长文件
}row;




int main(int argc, char *argv[]) {

  if(system("mkdir FILE")!=0)
        {perror("mkdir FILE");
        return 0;
        }
struct stat file_stat;
  for(int i=0;i<argc;i++)
  {
    printf("argc %d : %s \n",i,argv[i]);
  }

  void * start;
  MY_MBR my_mbr;
  int fd;
  fd=open(argv[1],O_RDONLY);
  if(fd==-1){
    perror("Can't find the file!");
    return 0;
  }
  if(fstat(fd,&file_stat)==-1){
    perror("Fstat error!");
    return 0;
  };
  
  start=mmap(0,file_stat.st_size ,PROT_READ,MAP_SHARED,fd,0);
  void *end=start+file_stat.st_size;
  void *now=start;
  if(start==(void*)-1){
    perror("Mmap!");
    return 0;
  }
  // printf("Success mmap!\n");
  close(fd);
  MBR *test1=(void *)now;
  my_mbr.sec_bit_num=*(int16_t *)test1->sec_bit_num;
  my_mbr.cluster_sec_num=*(int8_t *)test1->cluster_sec_num;
  my_mbr.res_sec_num=*(int16_t *)test1->res_sec_num;
  // printf("ori_res_sec_num 0:0x%x\n",test1->res_sec_num[0]);
  // printf("ori_res_sec_num 1:0x%x\n",test1->res_sec_num[1]);
  // printf("res_sec_num:0x%x\n",my_mbr.res_sec_num);
  my_mbr.fat_sec_num=*(int32_t *)test1->fat_sec_num;
  my_mbr.root_cluster=*(int32_t *)test1->root_cluster;
  //  printf("ori_root_cluster:0x%x\n",test1->root_cluster[0]);
  //  printf("ori_root_cluster:0x%x\n",test1->root_cluster[1]);
  //  printf("ori_root_cluster:0x%x\n",test1->root_cluster[2]);
  //  printf("ori_root_cluster:0x%x\n",test1->root_cluster[3]);
  //  printf("root_cluster:0x%x\n",my_mbr.root_cluster);

  //my_mbr.root_address=(void *)(int)(((int)my_mbr.res_sec_num+(int)my_mbr.fat_sec_num*2)*(int)my_mbr.sec_bit_num);
  my_mbr.difference=(int)(((int)my_mbr.res_sec_num+(int)my_mbr.fat_sec_num*2)*(int)my_mbr.sec_bit_num);//距离首地址的bit差距值
  my_mbr.root_address=(void*)(my_mbr.difference+start);//实际内存的位置;
  // printf("%d",my_mbr.difference/my_mbr.sec_bit_num);
  //  printf("root address:%p\n",my_mbr.root_address);

  

  int cnt=0;
  row *test2=start;//test2用来一行一行的遍历;
  void *row_end=(void *)(end-(void*)(&(test2[2].bit[0])-&(test2[0].bit[0])));//确定遍历的边界值;

  dir_s_item* short_item=start;
  dir_l_item* long_item=start;
  // printf(" testttt:%p",(void *)(end-(void*)(&(test2[1].bit[0])-&(test2[0].bit[0]))));
  int check=0;
  while(1){
    if((uint8_t)(0x20)==test2[cnt].bit[11]){//匹配短文件名的属性值;
      if(
        (uint8_t)(0x42)==test2[cnt].bit[8]&&
        (uint8_t)(0x4d)==test2[cnt].bit[9]&&
        (uint8_t)(0x50)==test2[cnt].bit[10]
      )//匹配BMP
      {
       FILE *fp=NULL;
      short_item=(void *)&(test2[cnt].bit[0]);
      if(short_item->file_name[0]==0xe5||short_item->file_name[0]==0x00){;}//如果这是一个被删除的或者没啥用的文件,那就恢复不能了;
      else{
      uint32_t cluster_num=
                            (short_item->h_b_cluster[0]<<2*8)+(short_item->h_b_cluster[1]<<3*8)+  
                            (short_item->l_b_cluster[0]<<0*8)+(short_item->l_b_cluster[1]<<1*8); //起始簇号;
  //                            uint16_t sec_bit_num;     //每一个扇区的字节数
  // uint8_t cluster_sec_num;  //每簇的扇区数
      void * file_address=(void *)((cluster_num-my_mbr.root_cluster)*my_mbr.cluster_sec_num*my_mbr.sec_bit_num+my_mbr.root_address);
int GG=0;//用来筛去一些不自信的东西;
      uint32_t file_length=*(int32_t *)short_item->length;
      char file_name[256+6];
      char temp_name[256+6];//仅仅用于短文件名；
      int length_of_filename=0; 
      length_of_filename=0;
      int finished=0;//用来判断有没有走长文件名那条路;
      for(int i=0;i<8;i++){
        if(short_item->file_name[i]!=0x20)
        temp_name[i]=(char)short_item->file_name[i];
        length_of_filename++;
      }
      temp_name[length_of_filename]='\0';

      if(temp_name[length_of_filename-1]=='1'&&temp_name[length_of_filename-2]=='~'){


      //短文件名已经找好了,现在来找长文件名,然后把短文件名的扩展名拼接在后面就行了
      //如果短文件名最后两个是~1那就有长文件名;
      //记得加一个判断！！！;

      void *for_fun=(void *)((void *)&(test2[cnt].bit[0])-(void *)(&(long_item[1].state[0])-&(long_item[0].state[0])));
      long_item=(dir_l_item *)for_fun;
      uint8_t now_checked=long_item->checked[0];
 
 
 
      int long_filename_cnt=0;
      
      
      

      if(long_item->attr[0]==0x0f){//这是个东西；
      // printf("reserved: 0x%x",long_item->reserved[0]);
      finished=0;
      long_filename_cnt=0;
do{
      //int success_flag=0;
      if(finished==0){
      for(int i=0;i<10;i+=2){
        if(long_item->name1[i]==0xff){
          finished=1;
          break;
        }
        temp_name[long_filename_cnt]=long_item->name1[i];
        // printf(" 1:0x%x ",temp_name[long_filename_cnt]);
        // printf("\n");
        long_filename_cnt+=1;
      }}
      if(finished==0){
      for(int i=0;i<12;i+=2){
        if(long_item->name2[i]==0xff){
          finished=1;
          break;
        }
        temp_name[long_filename_cnt]=long_item->name2[i];
        long_filename_cnt++;
      }
      }
      if(finished==0){
        for(int i=0;i<4;i+=2){
          if(long_item->name3[i]==0xff){
            finished=1;
            break;
          }
          temp_name[long_filename_cnt]=long_item->name3[i];
          long_filename_cnt++;
        }

      }
    
      for_fun=(void *)((void *)long_item-(void *)(&(long_item[1].state[0])-&(long_item[0].state[0])));
      long_item=(dir_l_item *)for_fun;

      // printf("long state: 0x%x\n",long_item[0].state[0]);

      }
      while(now_checked==long_item->checked[0]);
     finished=1; 
      
      if(temp_name[long_filename_cnt-1]=='m'){
        temp_name[long_filename_cnt]='p';
        long_filename_cnt++;
      }
      temp_name[long_filename_cnt]='\0';


      }else
      {
        GG=1;
      }
      
      }
      
      
      //strcpy(temp_name,(char*)short_item->file_name);
     // strcat(temp_name,"\0");
     if(GG==0){
      if(finished==0){
      sprintf(file_name,"./FILE/%s.%s",temp_name,short_item->extend_name);}
      else{

      sprintf(file_name,"./FILE/%s",temp_name);}
      
    //   printf(" file_address:%p\n",file_address);
    //  printf(" name: %s\n",file_name);

      fp=fopen(file_name,"w+");

      fwrite(file_address,file_length,1,fp);
      //memcpy(fp,file_address,file_length);
      fclose(fp);
      char systemcall[256+10+6];
      sprinf(systemcall,"sha1sum %s",file_name);
      
      system(systemcall);
      }
 //     printf("name : %s\n",file_name);




      // printf(" file_address:%p\n",file_address);
      // printf(" length:0x%x\n",file_length);


      // printf(" cluster_num :0x%x\n",cluster_num);
      // printf(" 0x%x,0x%x,0x%x,0x%x\n",short_item->h_b_cluster[0],short_item->h_b_cluster[1],short_item->l_b_cluster[0],short_item->l_b_cluster[1]);


      // printf(" short item: 0x%x\n",short_item->file_name[0]);

      // printf("check:0x%x",check);
      // printf("\n");
      }

      }
    }
    cnt++;
    check+=16;//check与hexdump前面的编号一样;
     if((void *)&test2[cnt].bit[0]>=row_end){
    //    printf("last:%p\n",(void*)&(test2[cnt].bit[0]));
      
      
       break;}
  
  }



  // printf("start:%p\n",start);
  // printf(" end:%p",end);
  // printf("test2:%p\n",(void*)&(test2[0].bit[0]));
  // printf("0x%x\n",my_mbr.sec_bit_num);
  // printf("0x%02x\n",test1[0].sec_bit_num[0]);
  // printf("0x%02x\n",test1[0].sec_bit_num[1]);
  return 0;
}
