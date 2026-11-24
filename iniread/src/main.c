#include <stdio.h>
#include <stdlib.h>

#include "../include/iniread.h"

int main() {
  INIREAD ini = iniread_create("test.ini", 0);

  int a;
  iniread_read_int(ini, "Header 01", "var_i", &a);
  printf("%d\n", a);

  iniread_destroy(&ini);
  return 0;
}
