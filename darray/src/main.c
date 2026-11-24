#include <stdio.h>
#include <stdlib.h>

#include "../include/darray.h"

int free_func(void *data) {
  if (!data)
    return 0;

  free(data);
  return 1;
}

int sort(const void *a, const void *b) { return *(int *)b - *(int *)a; }

int main() {
  DARRAY array = darray_create(sizeof(int), 0);

  for (int i = 0; i < 9; i++) {
    darray_append(array, &i);
  }

  int a;
  darray_reverse(array);
  for (int i = 0; i < darray_get_length(array); i++) {
    darray_get_element(array, i, &a);
    printf("%d\n", a);
  }

  darray_destroy(&array, NULL);
  return 0;
}
