#ifndef STACK_H
#define STACK_H

#include <stdint.h>

// Change Log
// Version  | DATE    | UPDATE
// 1.0        11/9/26   Initial version

// Flags
#define STACK_VERBOSE 0x1
#define STACK_UNBOUND_SIZE 0x2

// Error Codes
#define STACK_ERROR_SUCCESS 0x0
#define STACK_ERROR_NO_MEM 0x1
#define STACK_ERROR_NULL_INPUT 0x2
#define STACK_ERROR_FULL 0x3
#define STACK_ERROR_EMPTY 0x4

typedef struct stack_s *STACK;

STACK stack_create(uint32_t data_size, uint32_t stack_capacity, uint32_t flags);
uint32_t stack_destroy(STACK *stack);
uint32_t stack_clear(STACK stack);

uint32_t stack_push(STACK stack, void *data);
uint32_t stack_pop(STACK stack, void *output);

uint32_t stack_get_error();
char *stack_get_error_message();
char *stack_get_version();

#endif
