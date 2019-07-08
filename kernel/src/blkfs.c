 #include <klib.h>
 #include <blkfs.h>
 #include <vfs.h>

static int first_item_len(const char* path){
      int result=0;
      for(;path[result]!='\0'&& path[result]!='/';){
        result++;
      }
      return result;
}


//交给上层
void ext2_init(fs_t * fs,const char * name ,device_t* dev);
int ext2_create(ext2_t* ext2, int ridx, char* name, int mode);
void ext2_cd(ext2_t* ext2, char* dirname);
ssize_t ext2_write(ext2_t * ext2,int index,uint64_t offset,char * buf,uint32_t len);
ssize_t ext2_read(ext2_t* ext2, int index, uint64_t offset, char* buf,
                  uint32_t len);

uint32_t ext2_alloc_block(ext2_t*ext2);
uint32_t ext2_alloc_inode(ext2_t* ext2);
uint32_t ext2_research_file(ext2_t *ext2,char *path,int mode,
                           uint32_t * inode_num,uint32_t* block_num,uint32_t* dir_num);

char *init_str="Hello, World!\n";
//char zero[256];


void ext2_init(fs_t * fs,const char * name ,device_t* dev){
  
  ext2_t* ext2=(ext2_t*)fs->real_fs;
  memset(ext2, 0x00, sizeof(ext2_t));
  ext2->dev=dev;
  printf("Now create the blkfs---ext2fs\n");
  ext2->last_alloc_inode=100;
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
  //ext2->sb.volume_name=VOLUME_NAME;
  strcpy(ext2->sb.volume_name,VOLUME_NAME);
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
  ext2_rd_blockbitmap(ext2);
  ext2_rd_inodebitmap(ext2);
  for(int i=0;i<512;i++){
    ext2->inodebitmapbuf[i]=0;
    ext2->blockbitmapbuf[i]=0;
  }
  ext2_wr_inodebitmap(ext2);
  ext2_wr_blockbitmap(ext2);
  ext2->ind.mode = TYPE_DIR | RD_ABLE | WR_ABLE;
  ext2->ind.blocks = 0;
  ext2->ind.size = 2 * DIR_SIZE;  // . 和 ..
  ext2->ind.block[0] = ext2_alloc_block(ext2);
  ext2->ind.blocks+=1;

  ext2->current_dir=ext2_alloc_block(ext2);
  //strcpy(ext2->current_dir_name,"/");
  ext2_wr_ind(ext2,ext2->current_dir);

  ext2->dir[0].inode=ext2->dir[1].inode=ext2->current_dir;//根目录的. ..都是根目录
  ext2->dir[0].name_len=ext2->dir[1].name_len=0;//文件名n长度为0,隐藏文件;
//  ext2->dir[0].file_type=ext2->dir[1].file_type=TYPE_DIR;

  strcpy(ext2->dir[0].name,".");
  strcpy(ext2->dir[1].name,"..");
  ext2_wr_dir(ext2,ext2->ind.block[0]);

  int init_file=ext2_create(ext2,ext2->current_dir,"Hello_world.txt",TYPE_FILE|RD_ABLE|WR_ABLE);
  ext2_write(ext2,init_file,0,init_str,strlen(init_str));


  return ;

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
void ext2_inode_prepare(ext2_t *ext2,uint32_t index,uint32_t par,int mode){
  ext2_rd_ind(ext2,index);
 // if(file_type==TYPE_DIR){
   if(mode&TYPE_DIR){
    ext2->ind.size=2*DIR_SIZE;
    ext2->ind.blocks=1;
    ext2->ind.block[0]=ext2_alloc_block(ext2);
    ext2->dir[0].inode=index;
    ext2->dir[1].inode=par;//父亲节点的inode；
    //ext2->dir[0].file_type=ext2->dir[1].file_type=TYPE_DIR;
    ext2->dir[0].mode=ext2->dir[1].mode=TYPE_DIR;
    for(int k=2;k<DIR_AMUT;k++){
      ext2->dir[k].inode=0;
    }
    strcpy(ext2->dir[0].name,".");
    strcpy(ext2->dir[1].name,"..");
    ext2_wr_dir(ext2,ext2->ind.block[0]);
    //ext2->ind.mode=0x26;//maybe wrong;
    ext2->ind.mode=mode;
   // ext2->ind.file_type=file_type;
  }
  else if(mode&TYPE_FILE){
    ext2->ind.size=0;
    ext2->ind.blocks=0;
    ext2->ind.mode=mode;
  //  ext2->ind.file_type=file_type;
  }
  else{
    printf("Wrong type");
    assert(0);
  }
  ext2_wr_ind(ext2,index);

}


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
  ext2->inodebitmapbuf[cur] = ext2->inodebitmapbuf[cur] + con;
  ext2->last_alloc_inode = cur * 8 + flag + 1;
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

// //关于目录项的初始化;
// void ext2_dir_prepare(ext2_t * ext2,uint32_t index,uint32_t len,int type){
//  // enum { TYPE_FILE = 1, TYPE_DIR = 2 };
//   ext2_rd_ind(ext2,index);//取出相应index的inode;
//   if(type==TYPE_DIR){
//     ext2->ind.size=2*DIR_SIZE;      //存放 '.' '..'; 修改inode的大小;
//     ext2->ind.blocks=1;//只使用到一个块;
//     ext2->ind.block[0]=ext2_alloc_block(ext2);//新建块;
//     ext2->dir[0].inode=index;     //给目录项inode赋值;
//     ext2->dir[1].inode=ext2->current_dir;//给'..'的inode赋值为当前的目录;
//     ext2->dir[0].name_len=len;
//     ext2->dir[1].name_len=ext2->current_dir_name_len;
//     ext2->dir[0].file_type=ext2->dir[1].file_type=TYPE_DIR;

//     for(int i=2;i<DIR_AMUT;i++){
//       ext2->dir[i].inode=0;//将无用的目录项赋值为0;
//     }

//     strcpy(ext2->dir[0].name,".");
//     strcpy(ext2->dir[1].name,"..");

//     ext2_wr_dir(ext2,ext2->ind.block[0]);
//     ext2->ind.mode=0x26;/* drwxrwxrwx:目录 */
//     ext2->ind.file_type=TYPE_DIR;
//   }
//   else{//文件初始化;
//     ext2->ind.size=0;
//     ext2->ind.blocks=0;
//     ext2->ind.mode=0x17;
//     ext2->ind.file_type=TYPE_FILE;
//   }

//   ext2_wr_ind(ext2,index);

// }


//根据文件名文件类型,在当前目录里面寻找文件,并且把文件信息存到inode_num,block_num,dir_num 中;
//如果找不到,那么返回0,找到了就返回1;
uint32_t ext2_research_file(ext2_t *ext2,char *path,int mode,
                           uint32_t * inode_num,uint32_t* block_num,uint32_t* dir_num)
{
  ext2_rd_ind(ext2,ext2->current_dir);//获得当前的目录节点信息,记录到ext2->inode上面;

  for(uint32_t j=0;j<ext2->ind.blocks;j++){
    ext2_rd_dir(ext2,ext2->ind.block[j]);
    int len=first_item_len(path);
      for(uint32_t k=0;k<DIR_AMUT;){
        if(!ext2->dir[k].inode||
        !(ext2->dir[k].mode&mode)||//可能需要修改: mode不一样;0100 0010
            strncmp(ext2->dir[k].name,path,len)){
          k++;
        }
        else{
          ext2->current_dir=*inode_num=ext2->dir[k].inode;
          *block_num=j;
          *dir_num=k;
          return (len==strlen(path))
                     ? 1
                     : ext2_research_file(ext2,path+len+1,mode,inode_num,block_num,dir_num);
        }
      }
  }


  
  return 0;
  
}
int ext2_search_file(ext2_t* ext2, uint32_t idx) {
  for (int i = 0; i < MAX_OPEN_FILE_AMUT; i++)
    if (ext2->file_open_table[i] == idx) return 1;
  return 0;
}//看看这个文件有没有被打开过;

int ext2_lookup(filesystem_t *fs,const char *path,int mode){
  return 0;

}
int ext2_check(filesystem_t *fs){
  ext2_t* ext2=(ext2_t*)fs->real_fs;
  for(int i=0;i<100;i++){
  ext2_rd_ind(ext2,i);
          printf("%d ind.blocks:%x mode:%d size:%d\n",i,ext2->ind.blocks,ext2->ind.mode,ext2->ind.size);
  }

  return 0;
}
int ext2_readdir(filesystem_t *fs,int rinode_idx,int kth,vinode_t * buf){
  //根据rinode_idx 将这个目录项的第k个信息记录到buf里面;
  ext2_t* ext2=(ext2_t*)fs->real_fs;
  int cnt = 0;
  //ext2_check(fs);



  ext2_rd_ind(ext2, rinode_idx);
//   printf("rinode: %d, kth: %d\n", rinode_idx, kth);
  //        printf("dir0 name:%s\n",ext2->dir[0].name,ext2->dir[0].inode);
    //      printf("dir1 name:%s\n",ext2->dir[1].name,ext2->dir[1].inode);
      //    printf(":%d ind.blocks:%x mode:%d size:%d\n\n\n",rinode_idx,ext2->ind.blocks,ext2->ind.mode,ext2->ind.size);
          
  for (int i = 0; i < ext2->ind.blocks; i++) {
    ext2_rd_dir(ext2, ext2->ind.block[i]);
   // printf("%d: ",i);
    for (int k = 0; k < DIR_AMUT; k++) {
      if (ext2->dir[k].inode){//存在才返回;
      //    printf("dir name:%s inode:%d\n",ext2->dir[k].name,ext2->dir[k].inode);
        if (++cnt == kth) {
          strcpy(buf->name, ext2->dir[k].name);
          buf->rinode_index = ext2->dir[k].inode;
          buf->mode=ext2->dir[k].mode;
        //  printf("name:%s\n",buf->name);
          //buf->type= ext2->dir[k].file_type;
          return 1;
        }}
    }
  }

  return 0;
  
 
}

// void ext2_mkdir(ext2_t * ext2,char * dirname,int type,char*out){
//   int now_current_dir=ext2->current_dir;
//   uint32_t index,inode_num,block_num,dir_num;
//   int offset=sprintf(out,"");



    


// }


// ind_t* ext2_open(ext2_t * ext2,char * file_name){
//   uint32_t flag,i,j,k;//inode block dir
//   flag=ext2_research_file(ext2,file_name,TYPE_FILE,&i,&j,&k);

//   if(flag){
//     if(ext2_search_file(ext2,ext2->dir[k].inode)){
//       printf("Error:The file %s has opened!\n",file_name);
//     }
//     else{
//       flag=0;
//       while(ext2->file_open_table[flag]){
//         flag++;
//       }
//       ext2->file_open_table[flag]=ext2->dir[k].inode;
//       printf("File %s opened\n",file_name);
//     }
//   }
//   else{
//     printf("The file %s doesn't exist!\n");
//   }

// }







// // 交给shell的测试函数;
void ext2_cd(ext2_t* ext2, char* dirname) {
  uint32_t i, j, k, flag;
  if (!strcmp(dirname, "../")) dirname[2] = '\0';
  if (!strcmp(dirname, "./")) dirname[1] = '\0';
  flag = ext2_research_file(ext2, dirname, TYPE_DIR, &i, &j, &k);
  if (flag)
    ext2->current_dir = i;
  else
    printf("No directory: %s\n", dirname);
}

ssize_t ext2_read(ext2_t* ext2, int index, uint64_t offset, char* buf,
                  uint32_t len){
  //可能有问题;
  //读第index个inode的block;
  ext2_rd_ind(ext2,index);

  ssize_t result=0;
  //printf("ext2_read:here\n");
  int current_block=offset/BLK_SIZE;//之前用不到的block；
  int current_offset=offset-current_block*BLK_SIZE;//当前block的offset;
  //printf("current_block:%d,blocks: %d\n",current_block,ext2->ind.blocks);

  for(int i=current_block;i<ext2->ind.blocks;i++){
    ext2_rd_datablock(ext2,ext2->ind.block[i]);

    if(i==current_block){
      for(int j=0;j<ext2->ind.size-i*BLK_SIZE;j++){
        if(result==len||result+offset==ext2->ind.size){
          return result;
        }
        result+=sprintf(buf+result,"%c",ext2->datablockbuf[j+current_offset]);
      }
    }else{
      for(int j=0;j<ext2->ind.size-i*BLK_SIZE;j++){
        if(result==len||result+offset==ext2->ind.size){
          return result;
        }
        result+=sprintf(buf+result,"%c",ext2->datablockbuf[j]);
      }

    }
  }
  return result;
  //   int skip_blocks = offset / BLK_SIZE;
  // int first_offset = offset - skip_blocks * BLK_SIZE;

  // ext2_rd_ind(ext2, index);
  // int ret = 0;
  // for (int i = skip_blocks; i < ext2->ind.blocks; i++) {
  //   ext2_rd_datablock(ext2, ext2->ind.block[i]);
  //   if (i == skip_blocks)
  //     for (int j = 0; j < ext2->ind.size - i * BLK_SIZE; ++j) {
  //       if (ret == len || ret + offset == ext2->ind.size) return ret;
  //       ret += sprintf(buf + ret, "%c", ext2->datablockbuf[j + first_offset]);
  //     }
  //   else
  //     for (int j = 0; j < ext2->ind.size - i * BLK_SIZE; ++j) {
  //       if (ret == len || ret + offset == ext2->ind.size) return ret;
  //       ret += sprintf(buf + ret, "%c", ext2->datablockbuf[j]);
  //     }
  // }
  // return ret;
}
ssize_t ext2_write(ext2_t * ext2,int index,uint64_t offset,char * buf,uint32_t len){
  ext2_rd_ind(ext2,index);

  ssize_t result=0;
  int current_block=offset/BLK_SIZE;//之前用不到的block；
  int current_offset=offset-current_block*BLK_SIZE;//当前block的offset;
  int total_block=(len+offset+(BLK_SIZE-1))/BLK_SIZE;//总共需要的块数量;

  //可能需要修改mode;  
 if ((ext2->ind.mode & WR_ABLE) == 0) {
    printf("File can't be written!\n");
    return 0;
  }
  if(ext2->ind.blocks<=total_block){
    while(ext2->ind.blocks<total_block){
      ext2->ind.block[ext2->ind.blocks++]=ext2_alloc_block(ext2);
    }
  }else{
    while (ext2->ind.blocks>total_block)
    {
      ext2_remove_block(ext2,ext2->ind.block[--ext2->ind.blocks]);
    }
    
  }

  for (int n = current_block; n < total_block; n++) {
    if (n == current_block) {
      ext2_rd_datablock(ext2, ext2->ind.block[n]);
      for (int k = current_offset; result < len && k < BLK_SIZE; k++, result++)
        ext2->datablockbuf[k] = buf[result];
      ext2_wr_datablock(ext2, ext2->ind.block[n]);
    } else if (n != total_block - 1) {
      ext2_rd_datablock(ext2, ext2->ind.block[n]);
      for (int k = 0; result< len && k < BLK_SIZE; k++, result++)
        ext2->datablockbuf[k] = buf[result];
      ext2_wr_datablock(ext2, ext2->ind.block[n]);
    } else {
      ext2_rd_datablock(ext2, ext2->ind.block[n]);
      for (int k = 0; k < len - result; k++, result++)
        ext2->datablockbuf[k] = buf[result];
      ext2_wr_datablock(ext2, ext2->ind.block[n]);
    }
  }

  if(result!=len){
    printf("Error int ext2_write: result=%d,len=%d\n",result,len);
  }
  ext2->ind.size=offset+len;
  ext2_wr_ind(ext2,index);

  return result;
}
int ext2_create(ext2_t* ext2, int ridx, char* name, int mode) {
  ext2_rd_ind(ext2, ridx);

  assert(ext2->ind.size < 4096);
  int idx;
   printf("create: ridx[%d], size[%d]\n", ridx, ext2->ind.size);
  if (ext2->ind.size != ext2->ind.blocks * BLK_SIZE) {
    int i, j;
    for (i = 0; i < ext2->ind.blocks; i++) {
      ext2_rd_dir(ext2, ext2->ind.block[i]);
      for (j = 0; j < DIR_AMUT; j++)
        if (ext2->dir[j].inode == 0) goto CreateEnd;
    }
  CreateEnd:
    idx = ext2->dir[j].inode = ext2_alloc_inode(ext2);
   printf("1. new create: %d\n", idx);
    ext2->dir[j].mode = mode;
    ext2->dir[j].name_len = strlen(name);
    strcpy(ext2->dir[j].name, name);
    ext2_wr_dir(ext2, ext2->ind.block[i]);
  } else {
    ext2->ind.block[ext2->ind.blocks++] = ext2_alloc_block(ext2);
    ext2_rd_dir(ext2, ext2->ind.block[ext2->ind.blocks - 1]);
    idx = ext2->dir[0].inode = ext2_alloc_inode(ext2);
    ext2->dir[0].mode = mode;
    ext2->dir[0].name_len = strlen(name);
    strcpy(ext2->dir[0].name, name);
    for (int i = 1; i < DIR_AMUT; i++) ext2->dir[i].inode = 0;
    ext2_wr_dir(ext2, ext2->ind.block[ext2->ind.blocks - 1]);
  }
   printf("new create: %d\n", idx);
  ext2->ind.size += DIR_SIZE;  // now 32
  ext2_wr_ind(ext2, ridx);
  ext2_inode_prepare(ext2, idx, ridx, mode);
  return idx;
}
int ext2_remove(ext2_t* ext2,int index,char* name,int mode){
  ext2_rd_ind(ext2,index);
  int i, j, k = -1;
  for (i = 0; i < ext2->ind.blocks; i++) {
    ext2_rd_dir(ext2, ext2->ind.block[i]);
    for (j = 0; j < DIR_AMUT; j++)
      if (!strcmp(ext2->dir[j].name, name)) {
        k = ext2->dir[j].inode;
        break;
      }
    if (k != -1) break;
  }

  if (k == -1) return 1;
  //需要修改;
  if(mode&TYPE_DIR){
    ext2_rd_ind(ext2,ext2->dir[j].inode);

    if(ext2->ind.size==2*DIR_SIZE){
      assert(ext2->ind.blocks == 1);
      ext2->ind.size = ext2->ind.blocks = 0;
      ext2_remove_block(ext2, ext2->ind.block[0]);
      ext2_wr_ind(ext2, ext2->dir[j].inode);

      ext2_remove_inode(ext2, ext2->dir[j].inode);
      ext2->dir[j].inode = 0;
      ext2_wr_dir(ext2, ext2->ind.block[i]);

      ext2_rd_ind(ext2, index);
      ext2->ind.size -= DIR_SIZE;
      ext2_wr_ind(ext2, index);
      return 0;
    }
    else{
      //目录不是空的;错误信号1
      return 1;
    }
  }
  else{
    //删除文件;
    ext2_rd_ind(ext2, ext2->dir[j].inode);
    for (int m = 0; m < ext2->ind.blocks; m++) {
      ext2_remove_block(ext2, ext2->ind.block[m]);
      ext2->ind.block[m] = 0;
    }
    ext2_wr_ind(ext2, ext2->dir[j].inode);

    ext2_remove_inode(ext2, ext2->dir[j].inode);
    ext2->dir[j].inode = 0;
    ext2_wr_dir(ext2, ext2->ind.block[i]);

    ext2_rd_ind(ext2, index);
    ext2->ind.size -= DIR_SIZE;
    ext2_wr_ind(ext2, index);

    return 0;
  }

  return 0;
}

// uint32_t ext2_alloc_block(ext2_t* ext2);
// uint32_t ext2_alloc_inode(ext2_t* ext2);
// uint32_t ext2_reserch_file(ext2_t* ext2, char* name, int mode,
//                            uint32_t* inode_num, uint32_t* block_num,
//                            uint32_t* dir_num);
// void ext2_ind_prepare(ext2_t* ext2, uint32_t idx, uint32_t par, int mode);
// void ext2_remove_block(ext2_t* ext2, uint32_t del_num);
// int ext2_search_file(ext2_t* ext2, uint32_t idx);

// int ext2_create(ext2_t* ext2, int ridx, char* name, int mode);
// int ext2_remove(ext2_t* ext2, int ridx, char* name, int mode);
// void ext2_cd(ext2_t* ext2, char* dirname);
// // ssize_t ext2_read(ext2_t*, int, char*, uint32_t);
// ssize_t ext2_write(ext2_t*, int, uint64_t, char*, uint32_t);

// static int first_item_len(const char* path) {
//   int ret = 0;
//   for (; path[ret] != '\0' && path[ret] != '/';) ret++;
//   return ret;
// }

// #define ouput(str, ...) offset += sprintf(out + offset, str, ...)

// char* hello_str =
//     "#include <iostream> \nusing namespace std;\nint main(){\n  cout << "
//     "\"hello, world!\" << endl;\n  return 0;\n}\n";
// char* label_str = "This is a label!\n";
// char trash[4096];

// int ext2_init(filesystem_t* fs, const char* name, device_t* dev) {
//   ext2_t* ext2 = (ext2_t*)fs->real_fs;
//   memset(ext2, 0x00, sizeof(ext2_t));
//   ext2->dev = dev;
//   printf("Creating ext2: %s\n", name);
//   ext2->last_alloc_inode = 1;
//   ext2->last_alloc_block = 0;
//   for (int i = 0; i < MAX_OPEN_FILE_AMUT; i++) ext2->file_open_table[i] = 0;
//   ext2_rd_ind(ext2, 1);
//   ext2_rd_dir(ext2, 0);
//   ext2_rd_gd(ext2);  // gdt is changed here
//   ext2->gdt.block_bitmap = BLK_BITMAP;
//   ext2->gdt.inode_bitmap = IND_BITMAP;
//   ext2->gdt.inode_table = INDT_START;  // maye be no use
//   ext2->gdt.free_blocks_count = DATA_BLOCK_COUNT;
//   ext2->gdt.free_inodes_count = INODE_TABLE_COUNT;
//   ext2->gdt.used_dirs_count = 0;
//   ext2_wr_gd(ext2);

//   ext2_rd_blockbitmap(ext2);
//   ext2_rd_inodebitmap(ext2);
//   ext2->ind.mode = TYPE_DIR | RD_ABLE | WR_ABLE;
//   ext2->ind.blocks = 0;
//   ext2->ind.size = 2 * DIR_SIZE;  // origin 32, maybe wrong
//   ext2->ind.block[0] = ext2_alloc_block(ext2);
//   ext2->ind.blocks++;

//   ext2->current_dir = ext2_alloc_inode(ext2);
//   // printf("cur_dir: %d\n", ext2->current_dir);
//   ext2_wr_ind(ext2, ext2->current_dir);
//   // "." == ".." == root_dir
//   // root_dir with no name
//   ext2->dir[0].inode = ext2->dir[1].inode = ext2->current_dir;
//   ext2->dir[0].name_len = ext2->dir[1].name_len = 0;
//   strcpy(ext2->dir[0].name, ".");
//   strcpy(ext2->dir[1].name, "..");
//   ext2_wr_dir(ext2, ext2->ind.block[0]);

//   /* test */
//   int hello_cpp = ext2_create(ext2, ext2->current_dir, "hello.cpp",
//                               TYPE_FILE | RD_ABLE | WR_ABLE);
//   ext2_write(ext2, hello_cpp, 0, hello_str, strlen(hello_str));
//   strcpy(trash, name);
//   strcat(trash, ".txt");
//   int label = ext2_create(ext2, ext2->current_dir, trash,
//                           TYPE_FILE | RD_ABLE | WR_ABLE);
//   ext2_write(ext2, label, 0, label_str, strlen(label_str));
//   return 1;
// }

// int ext2_lookup(filesystem_t* fs, const char* path, int mode) { return 0; }
// int ext2_readdir(filesystem_t* fs, int ridx, int kth, vinode_t* buf) {
//   ext2_t* ext2 = (ext2_t*)fs->real_fs;
//   int cnt = 0;
//   ext2_rd_ind(ext2, ridx);
//    printf("ext2: %d \n", ext2->ind.blocks);
//   for (int i = 0; i < 7; i++) {
//     ext2_rd_dir(ext2, ext2->ind.block[i]);
//     for (int k = 0; k < DIR_AMUT; k++) {
//       if (ext2->dir[k].inode)
//         if (++cnt == kth) {
//           strcpy(buf->name, ext2->dir[k].name);
//           buf->rinode_index = ext2->dir[k].inode;
//           buf->mode = ext2->dir[k].mode;
//           return 1;
//         }
//     }
//   }
//   return 0;
// }

// uint32_t ext2_alloc_block(ext2_t* ext2) {
//   uint32_t cur = ext2->last_alloc_block / 8;
//   uint32_t con = 0x80; /* 0b10000000 */
//   int flag = 0;
//   if (ext2->gdt.free_blocks_count == 0) {
//     return -1;
//   }
//   ext2_rd_blockbitmap(ext2);
//   while (ext2->blockbitmapbuf[cur] == 0xff) {
//     if (cur == BLK_SIZE - 1)
//       cur = 0;  // restart from zero
//     else
//       cur++;  // try next
//   }
//   while (ext2->blockbitmapbuf[cur] & con) {
//     con = con / 2;
//     flag++;
//   }
//   ext2->blockbitmapbuf[cur] = ext2->blockbitmapbuf[cur] + con;
//   ext2->last_alloc_block = cur * 8 + flag;
//   ext2_wr_blockbitmap(ext2);
//   ext2->gdt.free_blocks_count--;
//   ext2_wr_gd(ext2);
//   return ext2->last_alloc_block;
// }

// uint32_t ext2_alloc_inode(ext2_t* ext2) {
//   uint32_t cur = (ext2->last_alloc_inode - 1) / 8;
//   uint32_t con = 0x80; /* 0b10000000 */
//   int flag = 0;
//   if (ext2->gdt.free_inodes_count == 0) {
//     return -1;
//   }
//   ext2_rd_inodebitmap(ext2);
//   while (ext2->inodebitmapbuf[cur] == 0xff) {
//     if (cur == BLK_SIZE - 1)
//       cur = 0;  // restart from zero
//     else
//       cur++;  // try next
//   }
//   while (ext2->inodebitmapbuf[cur] & con) {
//     con = con / 2;
//     flag++;
//   }
//   ext2->inodebitmapbuf[cur] = ext2->inodebitmapbuf[cur] + con;
//   ext2->last_alloc_inode = cur * 8 + flag + 1;
//   ext2_wr_inodebitmap(ext2);
//   ext2->gdt.free_inodes_count--;
//   ext2_wr_gd(ext2);
//   return ext2->last_alloc_inode;
// }

// uint32_t ext2_reserch_file(ext2_t* ext2, char* path, int mode, uint32_t* ninode,
//                            uint32_t* nblock, uint32_t* ndir) {
//   ext2_rd_ind(ext2, ext2->current_dir);
//   for (uint32_t j = 0; j < ext2->ind.blocks; j++) {
//     ext2_rd_dir(ext2, ext2->ind.block[j]);
//     int len = first_item_len(path);
//     for (uint32_t k = 0; k < DIR_AMUT;) {
//       if (!ext2->dir[k].inode ||
//           (ext2->dir[k].mode | mode) == ext2->dir[k].mode ||
//           strncmp(ext2->dir[k].name, path, len)) {
//         k++;
//       } else {
//         ext2->current_dir = *ninode = ext2->dir[k].inode;
//         *nblock = j;
//         *ndir = k;
//         return (len == strlen(path))
//                    ? 1
//                    : ext2_reserch_file(ext2, path + len + 1, mode, ninode,
//                                        nblock, ndir);
//       }
//     }
//   }
//   return 0;
// }

// void ext2_ind_prepare(ext2_t* ext2, uint32_t idx, uint32_t par, int mode) {
//   ext2_rd_ind(ext2, idx);
//   if (mode & TYPE_DIR) {
//     ext2->ind.size = 2 * DIR_SIZE;  // "." and ".."
//     ext2->ind.blocks = 1;
//     ext2->ind.block[0] = ext2_alloc_block(ext2);
//     ext2->dir[0].inode = idx;
//     ext2->dir[1].inode = par;
//     ext2->dir[0].mode = ext2->dir[1].mode = TYPE_DIR;
//     for (int k = 2; k < DIR_AMUT; k++) ext2->dir[k].inode = 0;
//     strcpy(ext2->dir[0].name, ".");
//     strcpy(ext2->dir[1].name, "..");
//     ext2_wr_dir(ext2, ext2->ind.block[0]);
//     ext2->ind.mode = mode;
//   } else if (mode & TYPE_FILE) {
//     ext2->ind.size = 0;
//     ext2->ind.blocks = 0;
//     ext2->ind.mode = mode;
//   } else {
//     assert(0);
//   }

//   ext2_wr_ind(ext2, idx);
// }

// void ext2_remove_block(ext2_t* ext2, uint32_t del_num) {
//   uint32_t tmp = del_num / 8;
//   ext2_rd_blockbitmap(ext2);
//   switch (del_num % 8) {
//     case 0:
//       ext2->blockbitmapbuf[tmp] &= 127;
//       break; /* 127 = 0b 01111111 */
//     case 1:
//       ext2->blockbitmapbuf[tmp] &= 191;
//       break; /* 191 = 0b 10111111 */
//     case 2:
//       ext2->blockbitmapbuf[tmp] &= 223;
//       break; /* 223 = 0b 11011111 */
//     case 3:
//       ext2->blockbitmapbuf[tmp] &= 239;
//       break; /* 239 = 0b 11101111 */
//     case 4:
//       ext2->blockbitmapbuf[tmp] &= 247;
//       break; /* 247 = 0b 11110111 */
//     case 5:
//       ext2->blockbitmapbuf[tmp] &= 251;
//       break; /* 251 = 0b 11111011 */
//     case 6:
//       ext2->blockbitmapbuf[tmp] &= 253;
//       break; /* 253 = 0b 11111101 */
//     case 7:
//       ext2->blockbitmapbuf[tmp] &= 254;
//       break; /* 254 = 0b 11111110 */
//   }
//   ext2_wr_blockbitmap(ext2);
//   ext2->gdt.free_blocks_count++;
//   ext2_wr_gd(ext2);
// }

// void ext2_remove_inode(ext2_t* ext2, uint32_t del_num) {
//   uint32_t tmp = (del_num - 1) / 8;
//   ext2_rd_inodebitmap(ext2);
//   switch ((del_num - 1) % 8) {
//     case 0:
//       ext2->inodebitmapbuf[tmp] &= 127;
//       break; /* 127 = 0b 01111111 */
//     case 1:
//       ext2->inodebitmapbuf[tmp] &= 191;
//       break; /* 191 = 0b 10111111 */
//     case 2:
//       ext2->inodebitmapbuf[tmp] &= 223;
//       break; /* 223 = 0b 11011111 */
//     case 3:
//       ext2->inodebitmapbuf[tmp] &= 239;
//       break; /* 239 = 0b 11101111 */
//     case 4:
//       ext2->inodebitmapbuf[tmp] &= 247;
//       break; /* 247 = 0b 11110111 */
//     case 5:
//       ext2->inodebitmapbuf[tmp] &= 251;
//       break; /* 251 = 0b 11111011 */
//     case 6:
//       ext2->inodebitmapbuf[tmp] &= 253;
//       break; /* 253 = 0b 11111101 */
//     case 7:
//       ext2->inodebitmapbuf[tmp] &= 254;
//       break; /* 254 = 0b 11111110 */
//   }
//   ext2_wr_inodebitmap(ext2);
//   ext2->gdt.free_inodes_count++;
//   ext2_wr_gd(ext2);
// }

// int ext2_search_file(ext2_t* ext2, uint32_t idx) {
//   for (int i = 0; i < MAX_OPEN_FILE_AMUT; i++)
//     if (ext2->file_open_table[i] == idx) return 1;
//   return 0;
// }

// void ext2_cd(ext2_t* ext2, char* dirname) {
//   uint32_t i, j, k, flag;
//   if (!strcmp(dirname, "../")) dirname[2] = '\0';
//   if (!strcmp(dirname, "./")) dirname[1] = '\0';
//   flag = ext2_reserch_file(ext2, dirname, TYPE_DIR, &i, &j, &k);
//   if (flag)
//     ext2->current_dir = i;
//   else
//     printf("No directory: %s\n", dirname);
// }

// ssize_t ext2_read(ext2_t* ext2, int ridx, uint64_t offset, char* buf,
//                   uint32_t len) {
//   int skip_blocks = offset / BLK_SIZE;
//   int first_offset = offset - skip_blocks * BLK_SIZE;

//   ext2_rd_ind(ext2, ridx);
//   int ret = 0;
//   for (int i = skip_blocks; i < ext2->ind.blocks; i++) {
//     ext2_rd_datablock(ext2, ext2->ind.block[i]);
//     if (i == skip_blocks)
//       for (int j = 0; j < ext2->ind.size - i * BLK_SIZE; ++j) {
//         if (ret == len || ret + offset == ext2->ind.size) return ret;
//         ret += sprintf(buf + ret, "%c", ext2->datablockbuf[j + first_offset]);
//       }
//     else
//       for (int j = 0; j < ext2->ind.size - i * BLK_SIZE; ++j) {
//         if (ret == len || ret + offset == ext2->ind.size) return ret;
//         ret += sprintf(buf + ret, "%c", ext2->datablockbuf[j]);
//       }
//   }
//   return ret;
// }

// ssize_t ext2_write(ext2_t* ext2, int ridx, uint64_t offset, char* buf,
//                    uint32_t len) {
//   int skip_blocks = offset / BLK_SIZE;
//   int first_offset = offset - skip_blocks * BLK_SIZE;

//   int need_blocks = (len + offset + (BLK_SIZE - 1)) / BLK_SIZE;

//   ssize_t ret = 0;
//   ext2_rd_ind(ext2, ridx);
//   if ((ext2->ind.mode & WR_ABLE) == 0) {
//     printf("File can't be writed!\n");
//     return 0;
//   }

//   if (ext2->ind.blocks <= need_blocks) {
//     while (ext2->ind.blocks < need_blocks)
//       ext2->ind.block[ext2->ind.blocks++] = ext2_alloc_block(ext2);
//   } else {
//     while (ext2->ind.blocks > need_blocks)
//       ext2_remove_block(ext2, ext2->ind.block[--ext2->ind.blocks]);
//   }

//   for (int n = skip_blocks; n < need_blocks; n++) {
//     if (n == skip_blocks) {
//       ext2_rd_datablock(ext2, ext2->ind.block[n]);
//       for (int k = first_offset; ret < len && k < BLK_SIZE; k++, ret++)
//         ext2->datablockbuf[k] = buf[ret];
//       ext2_wr_datablock(ext2, ext2->ind.block[n]);
//     } else if (n != need_blocks - 1) {
//       ext2_rd_datablock(ext2, ext2->ind.block[n]);
//       for (int k = 0; ret < len && k < BLK_SIZE; k++, ret++)
//         ext2->datablockbuf[k] = buf[ret];
//       ext2_wr_datablock(ext2, ext2->ind.block[n]);
//     } else {
//       ext2_rd_datablock(ext2, ext2->ind.block[n]);
//       for (int k = 0; k < len - ret; k++, ret++)
//         ext2->datablockbuf[k] = buf[ret];
//       ext2_wr_datablock(ext2, ext2->ind.block[n]);
//     }
//   }

//   if (ret != len) {
//     printf("something error, ret == %d, len == %d\n", ret, len);
//   }
//   ext2->ind.size = offset + len;
//   ext2_wr_ind(ext2, ridx);

//   return ret;
// }

// int ext2_create(ext2_t* ext2, int ridx, char* name, int mode) {
//   ext2_rd_ind(ext2, ridx);

//   assert(ext2->ind.size < 4096);
//   int idx;
//   // printf("create: ridx[%d], size[%d]\n", ridx, ext2->ind.size);
//   if (ext2->ind.size != ext2->ind.blocks * BLK_SIZE) {
//     int i, j;
//     for (i = 0; i < ext2->ind.blocks; i++) {
//       ext2_rd_dir(ext2, ext2->ind.block[i]);
//       for (j = 0; j < DIR_AMUT; j++)
//         if (ext2->dir[j].inode == 0) goto CreateEnd;
//     }
//   CreateEnd:
//     idx = ext2->dir[j].inode = ext2_alloc_inode(ext2);
//     ext2->dir[j].mode = mode;
//     ext2->dir[j].name_len = strlen(name);
//     strcpy(ext2->dir[j].name, name);
//     ext2_wr_dir(ext2, ext2->ind.block[i]);
//   } else {
//     ext2->ind.block[ext2->ind.blocks++] = ext2_alloc_block(ext2);
//     ext2_rd_dir(ext2, ext2->ind.block[ext2->ind.blocks - 1]);
//     idx = ext2->dir[0].inode = ext2_alloc_inode(ext2);
//     ext2->dir[0].mode = mode;
//     ext2->dir[0].name_len = strlen(name);
//     strcpy(ext2->dir[0].name, name);
//     for (int i = 1; i < DIR_AMUT; i++) ext2->dir[i].inode = 0;
//     ext2_wr_dir(ext2, ext2->ind.block[ext2->ind.blocks - 1]);
//   }
//   // printf("new create: %d\n", idx);
//   ext2->ind.size += DIR_SIZE;  // now 32
//   ext2_wr_ind(ext2, ridx);
//   ext2_ind_prepare(ext2, idx, ridx, mode);
//   return idx;
// }

// int ext2_remove(ext2_t* ext2, int ridx, char* name, int mode) {
//   ext2_rd_ind(ext2, ridx);

//   int i, j, k = -1;
//   for (i = 0; i < ext2->ind.blocks; i++) {
//     ext2_rd_dir(ext2, ext2->ind.block[i]);
//     for (j = 0; j < DIR_AMUT; j++)
//       if (!strcmp(ext2->dir[j].name, name)) {
//         k = ext2->dir[j].inode;
//         break;
//       }
//     if (k != -1) break;
//   }

//   if (k == -1) return 1;

//   if (mode & TYPE_DIR) {
//     ext2_rd_ind(ext2, ext2->dir[j].inode);
//     //  printf("The %d size is :%d\n", k, ext2->ind.size);
//     //  printf("The ridx = %d\n", ridx);
//     if (ext2->ind.size == 2 * DIR_SIZE) {
//       assert(ext2->ind.blocks == 1);
//       ext2->ind.size = ext2->ind.blocks = 0;
//       ext2_remove_block(ext2, ext2->ind.block[0]);
//       ext2_wr_ind(ext2, ext2->dir[j].inode);

//       ext2_remove_inode(ext2, ext2->dir[j].inode);
//       ext2->dir[j].inode = 0;
//       ext2_wr_dir(ext2, ext2->ind.block[i]);

//       ext2_rd_ind(ext2, ridx);
//       ext2->ind.size -= DIR_SIZE;
//       ext2_wr_ind(ext2, ridx);
//       return 0;  // SUCCESS
//     } else {
//       // printf("Dir is not empty! \n");
//       return 1;  // ERROR 1
//     }
//   } else {
//     // remove file
//     ext2_rd_ind(ext2, ext2->dir[j].inode);
//     for (int m = 0; m < ext2->ind.blocks; m++) {
//       ext2_remove_block(ext2, ext2->ind.block[m]);
//       ext2->ind.block[m] = 0;
//     }
//     ext2_wr_ind(ext2, ext2->dir[j].inode);

//     ext2_remove_inode(ext2, ext2->dir[j].inode);
//     ext2->dir[j].inode = 0;
//     ext2_wr_dir(ext2, ext2->ind.block[i]);

//     ext2_rd_ind(ext2, ridx);
//     ext2->ind.size -= DIR_SIZE;
//     ext2_wr_ind(ext2, ridx);

//     return 0;
//   }
// }

// void ext2_rd_sb(ext2_t* ext2) {
//   ext2->dev->ops->read(ext2->dev, DISK_START, &ext2->sb, SB_SIZE);
// }

// void ext2_wr_sb(ext2_t* ext2) {
//   ext2->dev->ops->write(ext2->dev, DISK_START, &ext2->sb, SB_SIZE);
// }

// void ext2_rd_gd(ext2_t* ext2) {
//   ext2->dev->ops->read(ext2->dev, GDT_START, &ext2->gdt, GD_SIZE);
// }

// void ext2_wr_gd(ext2_t* ext2) {
//   ext2->dev->ops->write(ext2->dev, GDT_START, &ext2->gdt, GD_SIZE);
// }

// void ext2_rd_ind(ext2_t* ext2, uint32_t i) {
//   uint32_t offset = INDT_START + (i - 1) * IND_SIZE;
//   ext2->dev->ops->read(ext2->dev, offset, &ext2->ind, IND_SIZE);
// }

// void ext2_wr_ind(ext2_t* ext2, uint32_t i) {
//   uint32_t offset = INDT_START + (i - 1) * IND_SIZE;
//   ext2->dev->ops->write(ext2->dev, offset, &ext2->ind, IND_SIZE);
// }

// void ext2_rd_dir(ext2_t* ext2, uint32_t i) {
//   uint32_t offset = DATA_BLOCK + i * BLK_SIZE;
//   ext2->dev->ops->read(ext2->dev, offset, &ext2->dir, BLK_SIZE);
// }

// void ext2_wr_dir(ext2_t* ext2, uint32_t i) {
//   uint32_t offset = DATA_BLOCK + i * BLK_SIZE;
//   ext2->dev->ops->write(ext2->dev, offset, &ext2->dir, BLK_SIZE);
// }

// void ext2_rd_blockbitmap(ext2_t* ext2) {
//   ext2->dev->ops->read(ext2->dev, BLK_BITMAP, &ext2->blockbitmapbuf, BLK_SIZE);
// }

// void ext2_wr_blockbitmap(ext2_t* ext2) {
//   ext2->dev->ops->write(ext2->dev, BLK_BITMAP, &ext2->blockbitmapbuf, BLK_SIZE);
// }

// void ext2_rd_inodebitmap(ext2_t* ext2) {
//   ext2->dev->ops->read(ext2->dev, IND_BITMAP, &ext2->inodebitmapbuf, BLK_SIZE);
// }

// void ext2_wr_inodebitmap(ext2_t* ext2) {
//   ext2->dev->ops->write(ext2->dev, IND_BITMAP, &ext2->inodebitmapbuf, BLK_SIZE);
// }

// void ext2_rd_datablock(ext2_t* ext2, uint32_t i) {
//   uint32_t offset = DATA_BLOCK + i * BLK_SIZE;
//   ext2->dev->ops->read(ext2->dev, offset, &ext2->datablockbuf, BLK_SIZE);
// }

// void ext2_wr_datablock(ext2_t* ext2, uint32_t i) {
//   uint32_t offset = DATA_BLOCK + i * BLK_SIZE;
//   ext2->dev->ops->write(ext2->dev, offset, &ext2->datablockbuf, BLK_SIZE);
// }
