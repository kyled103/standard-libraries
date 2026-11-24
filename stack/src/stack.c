#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/stack.h"

#define VERSION "1.0"

typedef struct stack_s {
  uint32_t capacity;
  uint32_t length;
  uint32_t data_size;
  uint8_t *data;
  uint32_t flags;
} *STACK;

static uint32_t stack_errno = STACK_ERROR_SUCCESS;
static char errmsg[64] = {0};

static uint32_t resize_stack(STACK stack);

STACK stack_create(uint32_t data_size, uint32_t stack_capacity,
                   uint32_t flags) {
  if (data_size == 0 || stack_capacity == 0) {
    if (flags & STACK_VERBOSE)
      printf("VERBOSE: NULL input.\n");

    stack_errno = STACK_ERROR_NULL_INPUT;
    return NULL;
  }

  STACK stack = malloc(sizeof(struct stack_s));
  if (!stack) {
    if (flags & STACK_VERBOSE)
      printf("VERBOSE: Malloc returned null.\n");

    stack_errno = STACK_ERROR_NO_MEM;
    return NULL;
  }

  if (flags & STACK_UNBOUND_SIZE)
    stack->capacity = 8;
  else
    stack->capacity = stack_capacity;

  stack->data = malloc(stack->capacity * data_size);
  if (!stack->data) {
    free(stack);
    if (flags & STACK_VERBOSE)
      printf("VERBOSE: Malloc returned null.\n");

    stack_errno = STACK_ERROR_NO_MEM;
    return NULL;
  }

  stack->data_size = data_size;
  stack->length = 0;
  stack->flags = flags;

  stack_errno = STACK_ERROR_SUCCESS;
  return stack;
}

uint32_t stack_destroy(STACK *stack) {
  if (!stack || !*stack) {
    stack_errno = STACK_ERROR_NULL_INPUT;
    return 0;
  }
  uint32_t flags = (*stack)->flags;

  if ((*stack)->flags & STACK_VERBOSE)
    printf("VERBOSE: Stack destroyed.\n");

  free((*stack)->data);
  free(*stack);
  *stack = NULL;

  if (flags & STACK_VERBOSE)
    printf("VERBOSE: Stack destroyed.\n");

  stack_errno = STACK_ERROR_SUCCESS;
  return 1;
}

uint32_t stack_clear(STACK stack) {
  if (!stack) {
    stack_errno = STACK_ERROR_NULL_INPUT;
    return 0;
  }

  stack->length = 0;

  if (stack->flags & STACK_VERBOSE)
    printf("VERBOSE: Stack cleared.\n");

  stack_errno = STACK_ERROR_SUCCESS;
  return 1;
}

uint32_t stack_push(STACK stack, void *data) {
  if (!stack || !data) {
    if (stack && stack->flags & STACK_VERBOSE)
      printf("VERBOSE: Null input parameter.\n");

    stack_errno = STACK_ERROR_NULL_INPUT;
    return 0;
  }

  if (stack->capacity == stack->length) {
    if (!(stack->flags & STACK_UNBOUND_SIZE)) {
      if (stack->flags & STACK_VERBOSE)
        printf("VERBOSE: Stack full.\n");

      stack_errno = STACK_ERROR_FULL;
      return 0;
    }

    if (!resize_stack(stack))
      return 0;
  }

  void *addr = &stack->data[stack->length * stack->data_size];
  memcpy(addr, data, stack->data_size);

  stack->length++;

  if (stack->flags & STACK_VERBOSE)
    printf("VERBOSE: Stack pushed.\n");

  stack_errno = STACK_ERROR_SUCCESS;
  return 1;
}

uint32_t stack_pop(STACK stack, void *output) {
  if (!stack || !output) {
    if (stack && stack->flags & STACK_VERBOSE)
      printf("VERBOSE: Null input parameter.\n");

    stack_errno = STACK_ERROR_NULL_INPUT;
    return 0;
  }

  if (stack->length == 0) {
    stack_errno = STACK_ERROR_EMPTY;
    return 0;
  }

  void *addr = &stack->data[(--stack->length) * stack->data_size];
  memcpy(output, addr, stack->data_size);

  if (stack->flags & STACK_VERBOSE)
    printf("VERBOSE: Stack poped.\n");

  stack_errno = STACK_ERROR_SUCCESS;
  return 1;
}

uint32_t stack_get_error() { return stack_errno; }

char *stack_get_error_message() {
  switch (stack_errno) {
  case STACK_ERROR_SUCCESS:
    sprintf(errmsg, "Error success");
    break;
  case STACK_ERROR_NO_MEM:
    sprintf(errmsg, "No memory");
    break;
  case STACK_ERROR_NULL_INPUT:
    sprintf(errmsg, "Null input parameter");
    break;
  case STACK_ERROR_FULL:
    sprintf(errmsg, "Stack is full");
    break;
  case STACK_ERROR_EMPTY:
    sprintf(errmsg, "Stack is empty");
    break;
  }

  return errmsg;
}

char *stack_get_version() { return VERSION; }

static uint32_t resize_stack(STACK stack) {
  uint32_t new_capacity = stack->capacity * 2;
  uint8_t *new_data = calloc(new_capacity, stack->data_size);
  if (!new_data) {
    if (stack->flags & STACK_VERBOSE)
      printf("VERBOSE: Failed to reallocate memory.\n");

    stack_errno = STACK_ERROR_NO_MEM;
    return 0;
  }

  memmove(new_data, stack->data, stack->capacity * stack->data_size);

  free(stack->data);
  stack->data = new_data;
  stack->capacity *= 2;
  if (stack->flags & STACK_VERBOSE)
    printf("VERBOSE: Stack reallocated to %u elements.\n", new_capacity);

  return 1;
}
