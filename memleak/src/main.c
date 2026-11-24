#include <stdlib.h>

#define MEMLEAK_IMPLEMENTATION
#include "../include/memleak.h"

int main() {
  memleak_init(0);

  for (int i = 0; i < 9; i++) {
    int *a = malloc(4);
  }

  return 0;
}
