#include <game.h>
#include<klib.h>

int w, h;
int cnt_w,cnt_h;//用来记录总的高度,宽度步数
int total=1;//用来记录蛇的长度
//暂时不考虑墙
void init_screen();
void init_snake();
void splash();//将整个画成白色
//int s_x,s_y;//测试用符号
//int w_x,w_y;//测试用的方向
void _snakemove(int key);
void _snakedraw();
int read_key2();

void draw_rect2(int x, int y, int w, int h, uint32_t color) ;

//字母w:30 s:44 d:45 a:43
int main() {
  // Operating nclude <graphics.h>system is a C program
  int next_frame=0;
  
  _ioe_init();
  init_screen(); 
  init_snake();
  splash();
  int key;
  printf("%d %d %d %d %d",w,h,SIDE,cnt_w,cnt_h);
  printf(" total: %d\n",total);
  while (1) {
    while(uptime()<next_frame);
    key = read_key2();
    if(key!=0)printf("%d\n",key);

    _snakemove(key);
    _snakedraw();
        //draw_rect2(cnt_w/2*SIDE, cnt_h/2*SIDE, SIDE, SIDE, 0x191970); // white
  //  puts(&key);
    next_frame+=1000/FPS;
  }
  return 0;
}
//字母w:30 s:44 d:45 a:43
//The following is just the test
//void snakemove(int key)
//{
//    int elderly_x,elderly_y;
//    elderly_x=s_x;
//    elderly_y=s_y;
//    switch(key)
//    {
//        case 0:break;
//        case 30:w_x=0;w_y=-1;break;
//        case 44:w_x=0;w_y=1;break;
//        case 45:w_x=1;w_y=0;break;
//        case 43:w_x=-1;w_y=0;break;
//    }
//
//    s_x+=w_x;
//    s_y+=w_y;
 //   
//        draw_rect2(elderly_x*SIDE, elderly_y*SIDE, SIDE, SIDE, _white); // white
//        draw_rect2(s_x*SIDE, s_y*SIDE, SIDE, SIDE, _black); // white
//}

void _snakemove(int key)
{
    //首先确定方向
    switch(key)
    {
        case 0:break;
        case 30:direction_x=0;direction_y=-1;break;
        case 44:direction_x=0;direction_y=1;break;
        case 45:direction_x=1;direction_y=0;break;
        case 43:direction_x=-1;direction_y=0;break;
    }

    //然后从蛇尾开始改变，更新位置
    
    tail_x=snake[total]._snake_x;
    tail_y=snake[total]._snake_y;
    snake[1]._snake_x+=direction_x;
    snake[1]._snake_y+=direction_y;
    for(int i=total;i>=2;--i)
    {
        snake[i]._snake_x=snake[i-1]._snake_x;
        snake[i]._snake_y=snake[i-1]._snake_y;
    }
    

}
void _snakedraw()
{
    for(int i=1;i<=total;i++)
    {
        draw_rect2(snake[i]._snake_x*SIDE, snake[i]._snake_y*SIDE, SIDE, SIDE, _black); 
    }
        draw_rect2(tail_x*SIDE, tail_y*SIDE, SIDE, SIDE, _white); 
}

int read_key2() {
  _DEV_INPUT_KBD_t event = { .keycode = _KEY_NONE };
  #define KEYNAME(key) \
    [_KEY_##key] = #key,
  static const char *key_names[] = {
    _KEYS(KEYNAME)
  };
  _io_read(_DEV_INPUT, _DEVREG_INPUT_KBD, &event, sizeof(event));
  if (event.keycode != _KEY_NONE && event.keydown) {
    puts("Key pressed: ");
    puts(key_names[event.keycode]);
    puts("\n");
  }
  return event.keycode;
}

//w是宽度,h是高
void init_screen() {
  _DEV_VIDEO_INFO_t info = {0};
  _io_read(_DEV_VIDEO, _DEVREG_VIDEO_INFO, &info, sizeof(info));
  w = info.width;
  h = info.height;
  cnt_w=w/SIDE;
  cnt_h=h/SIDE;
}

void init_snake()
{
    total=1;
    snake[1].number=1;
    snake[1]._snake_x=cnt_w/2;
    snake[1]._snake_y=cnt_h/2;
    direction_x=-1;
    direction_y=0;
    //以下用作测试
    total=2;
    
    snake[1]._snake_x=cnt_w/2+1;
    snake[1]._snake_y=cnt_h/2;
}
void draw_rect2(int x, int y, int w, int h, uint32_t color) {
  uint32_t pixels[w * h]; // WARNING: allocated on stack
  _DEV_VIDEO_FBCTL_t event = {
    .x = x, .y = y, .w = w, .h = h, .sync = 1,
    .pixels = pixels,
  };
  for (int i = 0; i < w * h; i++) {
    pixels[i] = color;
  }
  _io_write(_DEV_VIDEO, _DEVREG_VIDEO_FBCTL, &event, sizeof(event));
}

void splash() {
  for (int x = 0; x * SIDE <= w; x ++) {
    for (int y = 0; y * SIDE <= h; y++) {
        draw_rect2(x * SIDE, y * SIDE, SIDE, SIDE, _white); // white
    }
  }
}


