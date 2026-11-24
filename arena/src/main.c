#include <stdio.h>

#include "../../memleak/include/memleak.h"
#include "../include/arena.h"

int main() {
  memleak_init(0);
  ARENA arena = arena_create(MB(5), 0);

  arena_destroy(&arena);
  return 0;
}
