#ifndef ARENA_H
#define ARENA_H

#include <stdint.h>

// Change Log
// Version  | DATE      | UPDATE
// 1.0        11/19/26   Initial version

// Flags
#define ARENA_VERBOSE 0x1

// Error Codes
#define ARENA_ERROR_SUCCESS 0x0
#define ARENA_ERROR_NO_MEM 0x1
#define ARENA_ERROR_NULL_INPUT 0x2
#define ARENA_ERROR_FULL 0x3

#define KB(x) (1024 * x)
#define MB(x) (1024 * KB(x))

typedef struct arena_s *ARENA;

ARENA arena_create(uint32_t size, uint32_t flags);
uint32_t arena_destroy(ARENA *arena);

void *arena_push(ARENA arena, uint32_t size);

uint32_t arena_get_error();
char *arena_get_error_message();
char *arena_get_version();

#endif
