#include <stdio.h>

#include "../include/stack.h"

int main() {
  STACK stack = stack_create(sizeof(short), 8, STACK_UNBOUND_SIZE);

  for (int i = 0; i < 10; i++)
    stack_push(stack, &i);

  short b;
  while (stack_pop(stack, &b))
    printf("%d\n", b);

  stack_destroy(&stack);
  return 0;
}
