#include <klib.h>
#include <blkfs.h>
#include <vfs.h>
#define ouput(str, ...) offset += sprintf(out + offset, str, ...)
void ext2_init(fs_t * fs,const char * name ,device_t* dev){
  ext2_t* ext2=(ext2_t*)fs->fs;
  memset(ext2, 0x00, sizeof(ext2_t));
  ext2->dev=dev;
  printf("Now create the blkfs---ext2fs\n");
  ext2->last_alloc_block=1;
  ext2->last_alloc_block=0;
  for (int i = 0; i < MAX_OPEN_FILE_AMUT; i++){
    ext2->file_open_table[i] = 0;
    }

  //初始化缓冲区;
  ext2_rd_ind(ext2,1);
  ext2_rd_dir(ext2,0);//第零块数据区保留作为根目录;
  ext2_rd_sb(ext2);
  ext2_rd_gd(ext2);
  
  //初始化super block;
  ext2->sb.disk_size=DISK_SIZE;
  ext2->sb.blocks_per_group=BLK_PER_GROUP;
  ext2->sb.volume_name=VOLUME_NAME;
  ext2_wr_sb(ext2);

  //初始化gdt;

  ext2->gdt.block_bitmap=BLK_BITMAP;
  ext2->gdt.inode_bitmap=IND_BITMAP;
  ext2->gdt.inode_table=INDT_START;//初始化各个块地址;
  ext2->gdt.free_blocks_count=DATA_BLOCK_COUNT;//所有的都能用;
  ext2->gdt.free_inodes_count=INODE_TABLE_COUNT;//+1
  ext2->gdt.used_dirs_count=0;//本组目录个数为0;
  ext2_wr_gd(ext2);

  //初始化inode；
  ext2->current_dir=ext2_alloc_block(ext2);
  strcpy(ext2->current_dir_name,"/");
  ext2_wr_ind(ext2,ext2->current_dir);

  ext2->dir[0].inode=ext2->dir[1].inode=ext2->current_dir;//根目录的. ..都是根目录
  ext2->dir[0].name_len=ext2->dir[1].name_len=0;//文件名长度为0,隐藏文件;
  ext2->dir[0].file_type=ext2->dir[1].file_type=TYPE_DIR;

  strcpy(ext2->dir[0].name,".");
  strcpy(ext2->dir[1].name,"..");
  ext2_wr_dir(ext2,ext2->ind.block[0]);

}
//基本的小型操作
void ext2_rd_sb(ext2_t* ext2){
  ext2->dev->ops->read(ext2->dev,DISK_SIZE,&ext2->sb,SB_SIZE);//reload super block,从磁盘上面重载superblock到内存上面;
}
void ext2_wr_sb(ext2_t* ext2){
  ext2->dev->ops->write(ext2->dev, DISK_START, &ext2->sb, SB_SIZE);//将内存上面的东西写到磁盘上上面;
}
void ext2_rd_gd(ext2_t* ext2){
  ext2->dev->ops->read(ext2->dev, GDT_START, &ext2->gdt, GD_SIZE); //将磁盘上面的组信息重载到内存;
}
void ext2_wr_gd(ext2_t* ext2){
  ext2->dev->ops->write(ext2->dev, GDT_START, &ext2->gdt, GD_SIZE); //将内存上面的组信息写到磁盘上面;
}
void ext2_rd_ind(ext2_t* ext2, uint32_t i){
   uint32_t offset = INDT_START + (i - 1) * IND_SIZE;
  ext2->dev->ops->read(ext2->dev, offset, &ext2->ind, IND_SIZE); //从磁盘上面读取某一个inode的信息;
}
void ext2_wr_ind(ext2_t* ext2, uint32_t i){
  uint32_t offset = INDT_START + (i - 1) * IND_SIZE;
  ext2->dev->ops->write(ext2->dev, offset, &ext2->ind, IND_SIZE); //修改某一个inode到磁盘上;

}
void ext2_rd_dir(ext2_t* ext2, uint32_t i){
   uint32_t offset = DATA_BLOCK + i * BLK_SIZE;
  ext2->dev->ops->read(ext2->dev, offset, &ext2->dir, BLK_SIZE);//从第i块block上面读取目录项信息;
}
void ext2_wr_dir(ext2_t* ext2, uint32_t i){
  uint32_t offset = DATA_BLOCK + i * BLK_SIZE;
  ext2->dev->ops->write(ext2->dev, offset, &ext2->dir, BLK_SIZE);//将目录项信息写到第i块磁盘上面;
}
void ext2_rd_blockbitmap(ext2_t* ext2){
    ext2->dev->ops->read(ext2->dev, BLK_BITMAP, &ext2->blockbitmapbuf, BLK_SIZE);//将磁盘上面的blockbitmap内容读入到blockbitmapbuf上面;
}
void ext2_wr_blockbitmap(ext2_t* ext2){
   ext2->dev->ops->write(ext2->dev, BLK_BITMAP, &ext2->blockbitmapbuf, BLK_SIZE);//写bitmap信息到磁盘上面;
}
void ext2_rd_inodebitmap(ext2_t* ext2){
   ext2->dev->ops->read(ext2->dev, IND_BITMAP, &ext2->inodebitmapbuf, BLK_SIZE);//读inode bitmap 信息;
}
void ext2_wr_inodebitmap(ext2_t* ext2){
   ext2->dev->ops->write(ext2->dev, IND_BITMAP, &ext2->inodebitmapbuf, BLK_SIZE);//将inode bitmao 信息写到磁盘上面;
}
void ext2_rd_datablock(ext2_t* ext2, uint32_t i){
uint32_t offset = DATA_BLOCK + i * BLK_SIZE;
  ext2->dev->ops->read(ext2->dev, offset, &ext2->datablockbuf, BLK_SIZE);
}
void ext2_wr_datablock(ext2_t* ext2, uint32_t i){
   uint32_t offset = DATA_BLOCK + i * BLK_SIZE;
  ext2->dev->ops->write(ext2->dev, offset, &ext2->datablockbuf, BLK_SIZE);
}

//对磁盘的inode block进行操作:

//block的操作:
uint32_t ext2_alloc_block(ext2_t* ext2){//新建一个块;

  uint32_t cur=ext2->last_alloc_block/8;
  uint32_t con=0x80;                    // 128=0x80=0b1000 0000 

  int flag=0;
  if(ext2->gdt.free_blocks_count==0){
    printf("Error: There is no block to be allocked!!");
    return -1;
  }
  ext2_rd_blockbitmap(ext2);
  while(ext2->blockbitmapbuf[cur]==0xff){//直到扫到某一块是空的也就是不是0xff{
    if(cur==BLK_SIZE-1){
      cur=0;//从0开始重新搜;
    }
    else{
      cur++;
    }
  }
  while(ext2->blockbitmapbuf[cur]&con){//找到空的那一个数位;
    con=con/2;
    flag++;
  }
  //比如 cur=0010  con=1000 con=con/2的过程是1右移的过程;

  ext2->blockbitmapbuf[cur]=ext2->blockbitmapbuf[cur]+con;
  ext2->last_alloc_block=cur*8+flag;
  ext2_wr_blockbitmap(ext2);//将内存里面的blockbitmapbuf写进去;
  ext2->gdt.free_blocks_count--;
  ext2_wr_gd(ext2);
  return ext2->last_alloc_block;

}

void ext2_remove_block(ext2_t * ext2,uint32_t del_num){//del_num:要删除的块号;

  uint32_t tmp=del_num/8; //一个数记录8个block信息;
  ext2_rd_blockbitmap(ext2);
  switch(del_num%8){
    case 0:
      ext2->blockbitmapbuf[tmp] &= 127;
      break; /* 127 = 0b 01111111 */
    case 1:
      ext2->blockbitmapbuf[tmp] &= 191;
      break; /* 191 = 0b 10111111 */
    case 2:
      ext2->blockbitmapbuf[tmp] &= 223;
      break; /* 223 = 0b 11011111 */
    case 3:
      ext2->blockbitmapbuf[tmp] &= 239;
      break; /* 239 = 0b 11101111 */
    case 4:
      ext2->blockbitmapbuf[tmp] &= 247;
      break; /* 247 = 0b 11110111 */
    case 5:
      ext2->blockbitmapbuf[tmp] &= 251;
      break; /* 251 = 0b 11111011 */
    case 6:
      ext2->blockbitmapbuf[tmp] &= 253;
      break; /* 253 = 0b 11111101 */
    case 7:
      ext2->blockbitmapbuf[tmp] &= 254;
      break; /* 254 = 0b 11111110 */
  }

  ext2_wr_blockbitmap(ext2);
  ext2->gdt.free_blocks_count++;
  ext2_wr_gd(ext2);
}

//对inode的操作;
uint32_t ext2_alloc_inode(ext2_t * ext2){
  uint32_t cur=(ext2->last_alloc_inode-1)/8;
  uint32_t con=0x80;                         //0b 1000 0000

  int flag=0;
  if(ext2->gdt.free_inodes_count==0){
    printf("Error: There is no inode to be allocked!!");
    return -1;
  }
  ext2_rd_inodebitmap(ext2);
  while(ext2->inodebitmapbuf[cur]==0xff){
    if(cur==BLK_SIZE-1){
      cur=0;
    }
    else
    {
      cur++;
    }
  }

  while(ext2->inodebitmapbuf[cur]&con){
    con=con/2;
    flag++;
  }

  ext2->inodebitmapbuf[cur]+=con;
  ext2->last_alloc_inode=cur*8+flag+1;
  ext2_wr_inodebitmap(ext2);
  ext2->gdt.free_inodes_count--;
  ext2_wr_gd(ext2);
  return ext2->last_alloc_inode;
}

void ext2_remove_inode(ext2_t * ext2,uint32_t del_num){//del_num 要删除的inode 号;
uint32_t tmp = (del_num - 1) / 8;
  ext2_rd_inodebitmap(ext2);
  switch ((del_num - 1) % 8) {
    case 0:
      ext2->inodebitmapbuf[tmp] &= 127;
      break; /* 127 = 0b 01111111 */
    case 1:
      ext2->inodebitmapbuf[tmp] &= 191;
      break; /* 191 = 0b 10111111 */
    case 2:
      ext2->inodebitmapbuf[tmp] &= 223;
      break; /* 223 = 0b 11011111 */
    case 3:
      ext2->inodebitmapbuf[tmp] &= 239;
      break; /* 239 = 0b 11101111 */
    case 4:
      ext2->inodebitmapbuf[tmp] &= 247;
      break; /* 247 = 0b 11110111 */
    case 5:
      ext2->inodebitmapbuf[tmp] &= 251;
      break; /* 251 = 0b 11111011 */
    case 6:
      ext2->inodebitmapbuf[tmp] &= 253;
      break; /* 253 = 0b 11111101 */
    case 7:
      ext2->inodebitmapbuf[tmp] &= 254;
      break; /* 254 = 0b 11111110 */
  }
  ext2_wr_inodebitmap(ext2);
  ext2->gdt.free_inodes_count++;
  ext2_wr_gd(ext2);

}

//关于目录项的初始化;
void ext2_dir_prepare(ext2_t * ext2,uint32_t index,uint32_t len,int type){
 // enum { TYPE_FILE = 1, TYPE_DIR = 2 };
  ext2_rd_ind(ext2,index);//取出相应index的inode;
  if(type==2){
    ext2->ind.size=2*DIR_SIZE;      //存放 '.' '..'; 修改inode的大小;
    ext2->ind.blocks=1;//只使用到一个块;
    ext2->ind.block[0]=ext2_alloc_block(ext2);//新建块;
    ext2->dir[0].inode=index;     //给目录项inode赋值;
    ext2->dir[1].inode=ext2->current_dir;//给'..'的inode赋值为当前的目录;
    ext2->dir[0].name_len=len;
    ext2->dir[1].name_len=ext2->current_dir_name_len;
    ext2->dir[0].file_type=ext2->dir[1].file_type=TYPE_DIR;

    for(int i=2;i<DIR_AMUT;i++){
      ext2->dir[i].inode=0;//将无用的目录项赋值为0;
    }

    strcpy(ext2->dir[0].name,".");
    strcpy(ext2->dir[1].name,"..");

    ext2_wr_dir(ext2,ext2->ind.block[0]);
    ext2->ind.mode=01006;/* drwxrwxrwx:目录 */
  }
  else{//文件初始化;
    ext2->ind.size=0;
    ext2->ind.blocks=0;
    ext2->ind.mode=00407;
  }

  ext2_wr_ind(ext2,index);

}


//根据文件名文件类型,在当前目录里面寻找文件,并且把文件信息存到inode_num,block_num,dir_num 中;
//如果找不到,那么返回0,找到了就返回1;
uint32_t ext2_research_file(ext2_t *ext2,char *name,int file_type
                           uint32_t * inode_num,uint32_t* block_num,uint32_t* dir_num)
{
  ext2_rd_ind(ext2,ext2->current_dir);//获得当前的目录节点信息,记录到ext2->inode上面;

  for(uint32_t j=0;j<ext2->ind.blocks;j++){
    ext2_rd_dir(ext2,ext2->ind.block[j]);
      for(uint32_t k=0;k<DIR_AMUT;){
        if(!ext2->dir[k].inode||ext2->dir[k].file_type!=file_type){
          k++;
        }
        else{
          *inode_num=ext2->dir[k].inode;
          *block_num=j;
          *dir_num=k;
          return 1;
        }
      }
  }
  return 0;
  
}






