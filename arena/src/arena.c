#include <stdio.h>
#include <stdlib.h>

#include "../include/arena.h"

#define VERSION "1.0"

typedef struct arena_s {
  uint32_t capacity;
  uint32_t used;
  uint32_t flags;
  uint8_t data[];
} *ARENA;

static uint32_t arena_errno = ARENA_ERROR_SUCCESS;
static char errmsg[64] = {0};

ARENA arena_create(uint32_t size, uint32_t flags) {
  if (size == 0) {
    if (flags & ARENA_VERBOSE)
      printf("VERBOSE: NULL input.\n");

    arena_errno = ARENA_ERROR_NULL_INPUT;
    return NULL;
  }

  ARENA arena = malloc(sizeof(struct arena_s) + size);
  if (!arena) {
    if (flags & ARENA_VERBOSE)
      printf("VERBOSE: Malloc returned null.\n");

    arena_errno = ARENA_ERROR_NO_MEM;
    return NULL;
  }

  arena->capacity = size;
  arena->used = 0;
  arena->flags = flags;

  if (flags & ARENA_VERBOSE)
    printf("VERBOSE: Memory Arena created.\n");

  arena_errno = ARENA_ERROR_SUCCESS;
  return arena;
}

uint32_t arena_destroy(ARENA *arena) {
  if (!arena || !*arena) {
    arena_errno = ARENA_ERROR_NULL_INPUT;
    return 0;
  }

  uint8_t flags = (*arena)->flags;

  free(*arena);
  *arena = NULL;

  if (flags & ARENA_VERBOSE)
    printf("VERBOSE: Memory Arena destroyed.\n");

  arena_errno = ARENA_ERROR_SUCCESS;
  return 1;
}

void *arena_push(ARENA arena, uint32_t size) {
  if (!arena || size == 0) {
    if (arena && arena->flags & ARENA_VERBOSE)
      printf("VERBOSE: NULL input.\n");

    arena_errno = ARENA_ERROR_NULL_INPUT;
    return NULL;
  }

  if (arena->used + size > arena->capacity) {
    if (arena->flags & ARENA_VERBOSE)
      printf("VERBOSE: Arena out of capacity.\n");

    arena_errno = ARENA_ERROR_FULL;
    return NULL;
  }

  void *addr = &arena->data[arena->used];
  arena->used += size;

  if (arena->flags & ARENA_VERBOSE)
    printf("VERBOSE: Arena pushed with %u bytes.\n", size);

  arena_errno = ARENA_ERROR_SUCCESS;
  return addr;
}

uint32_t arena_get_error() { return arena_errno; }

char *arena_get_error_message() {
  switch (arena_errno) {
  case ARENA_ERROR_SUCCESS:
    sprintf(errmsg, "Error success");
    break;
  case ARENA_ERROR_NO_MEM:
    sprintf(errmsg, "No memory");
    break;
  case ARENA_ERROR_NULL_INPUT:
    sprintf(errmsg, "Null input parameter");
    break;
  case ARENA_ERROR_FULL:
    sprintf(errmsg, "Memory Arena is full");
    break;
  }

  return errmsg;
}

char *arena_get_version() { return VERSION; }
