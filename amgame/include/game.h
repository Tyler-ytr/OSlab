#include <am.h>
#include <amdev.h>

#define SIDE 16
#define FPS 60

static inline void puts(const char *s) {
  for (; *s; s++) _putc(*s);
}

//The following is the struct of the snake
typedef struct _Snake
{
    int number;
    int _snake_x;
    int _snake_y;
}_snake;

_snake snake[100]; //大胆猜测你玩不到100 QAQ


//The following is the color;
//Blue is the deepskyblue and the red is the salmon
#define _white 0xfffafa
#define _black 0x000000
#define _blue  0x00bfff
#define _red   0xfa8072
#define _green 0x90ee90
