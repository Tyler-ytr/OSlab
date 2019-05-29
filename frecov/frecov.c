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



typedef struct Directory_long_item{
  uint8_t state[1];//0-4字节顺序取值,5保留,第6个字节表示是长文件最后一个目录项;
  uint8_t name1[10];//长文件名unicode码1
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
  printf("Success mmap!\n");
  close(fd);
  MBR *test1=(void *)now;
  my_mbr.sec_bit_num=*(int16_t *)test1->sec_bit_num;

  int cnt=0;
  row *test2=start;//test2用来一行一行的遍历;
  void *row_end=(void *)(end-(void*)(&(test2[2].bit[0])-&(test2[0].bit[0])));//确定遍历的边界值;

  printf(" testttt:%p",(void *)(end-(void*)(&(test2[1].bit[0])-&(test2[0].bit[0]))));
  
  int check=0;
  while(1){
    if((uint8_t)(0x20)==test2[cnt].bit[11]){//匹配短文件名的属性值;
      if(
        (uint8_t)(0x42)==test2[cnt].bit[8]&&
        (uint8_t)(0x4d)==test2[cnt].bit[9]&&
        (uint8_t)(0x50)==test2[cnt].bit[10]
      )//匹配BMP


      printf("0x%x",check);
      printf("\n");

    }cnt++;check+=16;
    if((void *)&test2[cnt].bit[0]>=row_end){
       printf("last:%p\n",(void*)&(test2[cnt].bit[0]));
      
      
      break;}
  
  }
      // printf("test2:%p\n",(void*)&(test2[cnt].bit[0]));
      // printf(" row[8]:0x%x",test2[cnt].bit[0]);
      // printf(" row[9]:0x%x",test2[cnt].bit[1]);
      // printf(" row[10]:0x%x",test2[cnt].bit[2]);


  printf("start:%p\n",start);
  printf(" end:%p",end);
  printf("test2:%p\n",(void*)&(test2[0].bit[0]));
  printf("0x%x\n",my_mbr.sec_bit_num);
  printf("0x%02x\n",test1[0].sec_bit_num[0]);
  printf("0x%02x\n",test1[0].sec_bit_num[1]);
  return 0;
}
