#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MEMLEAK_VERBOSE 0x1

typedef struct {
  void *ptr;
  size_t size;
  char file[32];
  size_t line;
} MEM_INFO;

typedef struct {
  size_t capacity;
  size_t length;
  size_t total_bytes;
  MEM_INFO *data;
} ARRAY;

static int __flags = 0;
static ARRAY __array = {0};

/* Dynaimc Array Implementation */

static int array_init() {
  __array.data = malloc(8 * sizeof(MEM_INFO));
  if (!__array.data) {
    if (__flags & MEMLEAK_VERBOSE)
      printf("VERBOSE: Mem Leak failed to initialize.\n");

    return 0;
  }

  __array.length = 0;
  __array.capacity = 8;
  __array.total_bytes = 0;

  if (__flags & MEMLEAK_VERBOSE)
    printf("VERBOSE: Mem Leak Initialized.\n");

  return 1;
}

static void array_destroy() {
  free(__array.data);

  if (__flags & MEMLEAK_VERBOSE)
    printf("VERBOSE: Mem Leak Destroyed.\n");
}

static void array_add(void *ptr, size_t size, char *file, int line) {
  if (__array.length == __array.capacity) {
    __array.capacity *= 2;
    void *temp = realloc(__array.data, __array.capacity * sizeof(MEM_INFO));
    if (!temp) {
      if (__flags & MEMLEAK_VERBOSE) {
        printf("VERBOSE: Failed to increase internal dynamic array.\n");
#ifdef _WIN32
        printf("VERBOSE: 0x%p with %lld in %s on line %d was not added.\n", ptr,
               size, file, line);
#else
        printf("VERBOSE: %p with %lld in %s on line %d was not added.\n", ptr,
               size, file, line);
#endif
        printf("VERBOSE: Correct allocated address was still returned.\n");
      }

      return;
    }

    __array.data = temp;
  }

  MEM_INFO *mi = (MEM_INFO *)&__array.data[__array.length];

  mi->ptr = ptr;
  mi->size = size;
  strncpy(mi->file, file, 31);
  mi->line = line;

  __array.total_bytes += size;
  __array.length++;

  if (__flags & MEMLEAK_VERBOSE)
#ifdef _WIN32
    printf("VERBOSE: 0x%p pushed with %lld bytes in %s on line %d\n", ptr, size,
           file, line);
#else
    printf("VERBOSE: %p pushed with %lld bytes in %s on line %d\n", ptr, size,
           file, line);
#endif
}

static void array_overwrite(void *old, void *new, size_t size, char *file,
                            int line) {
  for (int i = 0; i < __array.length; i++) {
    MEM_INFO *mi = &__array.data[i];

    if (mi->ptr == old) {
      __array.total_bytes += (size - mi->size);

      mi->ptr = new;
      mi->size = size;
      strncpy(mi->file, file, 31);
      mi->line = line;

      if (__flags & MEMLEAK_VERBOSE)
#ifdef _WIN32
        printf("VERBOSE: 0x%p reallocated to 0x%p with %lld bytes in %s on "
               "line %d\n",
               old, new, size, file, line);
#else
        printf(
            "VERBOSE: %p reallocated to %p with %lld bytes in %s on line %d\n",
            old, new, size, file, line);
#endif
      return;
    }
  }
}

static void array_remove(void *ptr) {
  for (int i = 0; i < __array.length; i++) {
    MEM_INFO *mi = &__array.data[i];

    if (mi->ptr == ptr) {
      if (__flags & MEMLEAK_VERBOSE)
#ifdef _WIN32
        printf("VERBOSE: 0x%p removed with %lld bytes from %s on line %lld\n",
               ptr, mi->size, mi->file, mi->line);
#else
        printf("VERBOSE: 0x%p pushed with %lld bytes in %s on line %d\n", ptr,
               mi->size, mi->file, mi->line);
#endif

      __array.total_bytes -= mi->size;
      memmove(mi, mi + 1, (__array.length - i - 1) * sizeof(MEM_INFO));

      __array.length--;
      return;
    }
  }
}

/* End of Dynaimc Array Implementation */

#include "../include/memleak.h"

static void memleak_detect();

int memleak_init(int flags) {
  __flags = flags;
  atexit(memleak_detect);

  return array_init();
}

void *memleak_malloc(size_t size, char *file, int line) {
  void *ptr = malloc(size);
  if (!ptr)
    return NULL;

  array_add(ptr, size, file, line);
  return ptr;
}

void memleak_free(void *ptr) {
  if (!ptr)
    return;

  array_remove(ptr);
  free(ptr);
}

void *memleak_calloc(size_t nmemb, size_t size, char *file, int line) {
  void *ptr = calloc(nmemb, size);

  if (!ptr)
    return NULL;

  array_add(ptr, nmemb * size, file, line);
  return ptr;
}

void *memleak_realloc(void *ptr, size_t size, char *file, int line) {
  void *ptr_ = realloc(ptr, size);
  if (!ptr_)
    return NULL;

  array_overwrite(ptr, ptr_, size, file, line);
  return ptr_;
}

#ifndef _WIN32
void *memleak_reallocarray(void *ptr, size_t nmemb, size_t size, char *file,
                           int line) {
  void *ptr_ = reallocarray(ptr, nmemb, size);

  if (!ptr_)
    return NULL;

  array_overwrite(ptr, ptr_, size, file, line);
  return ptr_;
}
#endif

static void memleak_detect() {
  printf("Total bytes leaked: %lld\n", __array.total_bytes);

  if (__array.total_bytes == 0)
    return;

  int color = 0;
  printf("|      Address%*sSize%*sLocation%*sLine   |\n", 12, " ", 8, " ", 32,
         " ");
  for (int i = 0; i < __array.length; i++) {
    MEM_INFO *mi = &__array.data[i];

    char buffer[64] = {0};

    printf("| ");

    if (color)
      printf("\e[39m");
    else
      printf("\e[90m");

    printf("%03u  ", i + 1);
#ifdef _WIN32
    sprintf(buffer, "0x%p", mi->ptr);
#else
    sprintf(buffer, "%p", mi->ptr);
#endif
    printf("%-19s", buffer);
    sprintf(buffer, "%lld", mi->size);
    printf("%-*s", 12, buffer);
    printf("%-*s", 40, mi->file);
    sprintf(buffer, "%lld", mi->line);
    printf("%-*s", 6, buffer);

    color ^= 1;

    printf("\e[0m");
    printf(" |\n");
  }

  array_destroy();
}
