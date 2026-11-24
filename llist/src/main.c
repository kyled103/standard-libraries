#include "../include/llist.h"

int sort_func(const void *a, const void *b) {
  int *A = (int *)a;
  int *B = (int *)b;

  return *B - *A;
}

int main() {
  LLIST list = llist_create(sizeof(int), 0);

  for (int i = 0; i < 500; i++)
    llist_append(list, &i);

  llist_select_sort(list, sort_func);

  llist_destroy(&list, NULL);
  return 0;
}
