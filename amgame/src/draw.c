#include "game.h"

extern void draw_rect2(int x,int y,int w,int h,uint32_t color);
extern char font8x8_basic[128][8];

void draw_character(char ch, int x, int y,int color)
{
    char *p=font8x8_basic[(int)ch];
    for(int i=0;i<8;i++)
    {
        for(int j=0;j<8;j++)
        {
            if((p[i]>>j)&1)
            {
                 draw_rect2(y*SIDE+j*2,x*SIDE+i*2,2,2, color);
                
            }
        }
    }
}




