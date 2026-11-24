#ifndef DARRAY_H
#define DARRAY_H

#include <stdint.h>

// Change Log
// Version  | DATE      | UPDATE
// 1.0        11/10/26   Initial version

// Flags
#define DARRAY_VERBOSE 0x1

// Error Codes
#define DARRAY_ERROR_SUCCESS 0x0
#define DARRAY_ERROR_NO_MEM 0x1
#define DARRAY_ERROR_NULL_INPUT 0x2
#define DARRAY_ERROR_BAD_INDEX 0x3
#define DARRAY_ERROR_FREE_MEMBER_FAIL 0x4

typedef struct darray_s *DARRAY;

DARRAY darray_create(uint32_t data_size, uint32_t flags);
uint32_t darray_destroy(DARRAY *darray, int (*free_func)(void *data));
uint32_t darray_clear(DARRAY darray, int (*free_func)(void *data));

uint32_t darray_append(DARRAY darray, void *data);
uint32_t darray_prepend(DARRAY darray, void *data);
uint32_t darray_insert(DARRAY darray, uint32_t index, void *data);

uint32_t darray_remove(DARRAY darray, uint32_t index,
                       int (*free_func)(void *data));

uint32_t darray_replace(DARRAY darray, uint32_t index, void *data,
                        int (*free_func)(void *data));

uint32_t darray_swap(DARRAY darray, uint32_t index_a, uint32_t index_b);
uint32_t darray_sort(DARRAY darray,
                     int (*sort_func)(const void *a, const void *b));
uint32_t darray_reverse(DARRAY darray);

uint32_t darray_get_element(DARRAY darray, uint32_t index, void *output);

uint32_t darray_get_length(DARRAY darray);
uint32_t darray_get_error();
char *darray_get_error_message();
char *darray_get_version();

#endif
