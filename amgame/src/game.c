#include <game.h>
#include<klib.h>

int w, h;
int cnt_w,cnt_h;//用来记录总的高度,宽度步数
int total=1;//用来记录蛇的长度
//暂时不考虑墙
void init_screen();
void splash();//将整个画成黑白
int s_x,s_y;//测试用符号
int w_x,w_y;//测试用的方向
void snakemove(int key);
int read_key2();

void draw_rect2(int x, int y, int w, int h, uint32_t color) ;

//字母w:30 s:44 d:45 a:43
int main() {
  // Operating nclude <graphics.h>system is a C program
  int next_frame=0;
  
  _ioe_init();
  init_screen();
  splash();
  s_x=cnt_w/2;
  s_y=cnt_h/2;
  w_x=1;
  w_y=0;
  int key;
  printf("%d %d %d %d %d",w,h,SIDE,cnt_w,cnt_h);
  printf(" total: %d\n",total);
  while (1) {
    while(uptime()<next_frame);
    key = read_key2();
    if(key!=0)printf("%d\n",key);

    snakemove(key);
        //draw_rect2(cnt_w/2*SIDE, cnt_h/2*SIDE, SIDE, SIDE, 0x191970); // white
  //  puts(&key);
    next_frame+=1000/FPS;
  }
  return 0;
}
//字母w:30 s:44 d:45 a:43

void snakemove(int key)
{
    int elderly_x,elderly_y;
    elderly_x=s_x;
    elderly_y=s_y;
    switch(key)
    {
        case 0:break;
        case 30:w_x=0;w_y=1;break;
        case 44:w_x=0;w_y=-1;break;
        case 45:w_x=1;w_y=0;break;
        case 43:w_x=-1;w_y=0;break;
    }

    s_x+=w_x;
    s_y+=w_y;
    
        draw_rect2(elderly_x*SIDE, elderly_y*SIDE, SIDE, SIDE, _white); // white
        draw_rect2(s_x*SIDE, s_y*SIDE, SIDE, SIDE, _black); // white




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


