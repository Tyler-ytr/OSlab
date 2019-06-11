#ifndef _BLKFS_H
#define _BLKFS_H

#include <devices.h>

#define BLK_SIZE (512)                     //block size=512
#define DISK_START (0 * BLK_SIZE)          // disk offset
#define GDT_START (1 * BLK_SIZE)           // group_desc table offset
#define BLK_BITMAP (2 * BLK_SIZE)          // block bitmap offset
#define IND_BITMAP (3 * BLK_SIZE)          // inode bitmap offset
#define INDT_START (4 * BLK_SIZE)          // inode table offset
#define DATA_BLOCK ((4 + 512) * BLK_SIZE)  // data block offset
#define DATA_SIZE (4096)                   // data block size(blocks)
#define DISK_SIZE (DATA_SIZE + 512)        // disk size(blocks)
#define EXT2_N_BLOCKS (12)                 // ext2 inode blocks
#define VOLUME_NAME "EXT2FS"               // volume name


#define INODE_TABLE_COUNT 4096
#define DATA_BLOCK_COUNT 4096  


enum { TYPE_FILE = 1, TYPE_DIR = 2 };

struct super_block {
  /* super block, 32 bytes */
  char volume_name[16];     //卷名
  uint16_t disk_size;       //磁盘总大小
  uint16_t blocks_per_group;//每一组的块数
  uint16_t inodes_per_blocks;//每一组的inode数量
  char pad[10];               //填充
};

struct group_desc {
  /* block group descriptor, 32 bytes */
  char volume_name[16];        //卷名
  uint16_t block_bitmap;       //保存块位图的块号;
  uint16_t inode_bitmap;       //保存索引节点位图的块号;
  uint16_t inode_table;        //索引结点表的起始块号;
  uint16_t free_blocks_count;  //本组空闲块的个数;
  uint16_t free_inodes_count;  //本组空闲索引节点的个数;
  uint16_t used_dirs_count;    //本组目录的个数;
  char pad[4];                  //填充
};

struct inode {
  /* inode, 64 bytes */
  uint16_t mode;                  //文件类型以及访问的权限'
  uint32_t blocks;                //文件的数据块个数(0-7)
  uint32_t size;                  // the size of file 
  uint32_t block[EXT2_N_BLOCKS];  // direct or indirect blocks
  char pad[6];
};

struct directory {
  /* directory entry, 32 bytes */
  uint32_t inode;                  //索引节点号
  uint16_t rec_len;                //目录项长度
  uint8_t name_len;                //文件名长度
  uint8_t file_type;               //文件类型 (1: 普通文件， 2: 目录.. )
  char name[16];                   //文件名
  char pad[8];                     //填充
};
typedef struct super_block sb_t;
typedef struct group_desc gd_t;
typedef struct inode ind_t;
typedef struct directory dir_t;

#define SB_SIZE (sizeof(sb_t))
#define GD_SIZE (sizeof(gd_t))
#define IND_SIZE (sizeof(ind_t))
#define DIR_SIZE (sizeof(dir_t))
#define DIR_AMUT (BLK_SIZE / DIR_SIZE)
#define MAX_OPEN_FILE_AMUT (16)

struct ext2 {
  struct super_block sb;        //超级快
  struct group_desc gdt;        //组
  struct inode ind;             //inode块
  struct directory dir[DIR_AMUT];//目录项
  unsigned char blockbitmapbuf[BLK_SIZE];//bitmap
  unsigned char inodebitmapbuf[BLK_SIZE];//inodebitmap
  unsigned char datablockbuf[BLK_SIZE];
  uint32_t last_alloc_block;//最近分配的节点号
  uint32_t last_alloc_inode;//最近分配的数据块号
  uint32_t current_dir;//当前目录的节点号
  uint32_t current_dir_name_len;//当前目录的name长度
  uint32_t file_open_table[MAX_OPEN_FILE_AMUT];//
  char current_dir_name[256];
  device_t* dev;//设备
};

typedef struct ext2 ext2_t;


void ext2_rd_sb(ext2_t* ext2);
void ext2_wr_sb(ext2_t* ext2);
void ext2_rd_gd(ext2_t* ext2);
void ext2_wr_gd(ext2_t* ext2);
void ext2_rd_ind(ext2_t* ext2, uint32_t i);
void ext2_wr_ind(ext2_t* ext2, uint32_t i);
void ext2_rd_dir(ext2_t* ext2, uint32_t i);
void ext2_wr_dir(ext2_t* ext2, uint32_t i);
void ext2_rd_blockbitmap(ext2_t* ext2);
void ext2_wr_blockbitmap(ext2_t* ext2);
void ext2_rd_inodebitmap(ext2_t* ext2);
void ext2_wr_inodebitmap(ext2_t* ext2);
void ext2_rd_datablock(ext2_t* ext2, uint32_t i);
void ext2_wr_datablock(ext2_t* ext2, uint32_t i);

void ext2_ls(ext2_t* ext2, char* dirname, char* out);



#endif 