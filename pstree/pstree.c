#include <stdio.h>
#include <assert.h>

int main(int argc, char *argv[]) {
  printf("Hello, World!\n");
  int i;
  for (i = 0; i < argc; i++) {
    assert(argv[i]); // always true
    printf("argv[%d] = %s\n", i, argv[i]);
  }
  printf("I am here\n");
  assert(!argv[argc]); // always true
  printf("I am here\n");
  return 0;
}
