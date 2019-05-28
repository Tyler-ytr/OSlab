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

 typedef struct mbr{
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
  uint8_t vol_num[1];//卷序列号;
  uint8_t vol_ascii[11];//卷标(ASCII);
  uint8_t fs_ascii[8];//文件系统的ASCII码;
  uint8_t useless3[410];
  uint8_t end[2];//结束标志;




 }MBR;





int main(int argc, char *argv[]) {
struct stat file_stat;
  for(int i=0;i<argc;i++)
  {
    printf("argc %d : %s \n",i,argv[i]);
  }

  void * start;
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
  if(start==(void*)-1){
    perror("Mmap!");
    return 0;
  }
  printf("Success mmap!\n");
  close(fd);
  MBR *test1=(void *)start;

  
  printf("0x%02x\n",test1[0].mark[0]);
  printf("0x%02x\n",test1[0].mark[1]);

  return 0;
}
