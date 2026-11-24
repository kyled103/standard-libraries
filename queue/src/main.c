#include "../include/queue.h"
#include <stdio.h>

int main() {
  QUEUE queue = queue_create(sizeof(int), 0);

  for (int i = 0; i < 100; i++)
    queue_push(queue, &i);

  int a;
  while (queue_pop(queue, &a))
    printf("%d\n", a);
  ;

  queue_destroy(&queue);
  return 0;
}
