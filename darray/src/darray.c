#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/darray.h"

#define VERSION "1.0"

typedef struct darray_s {
  uint32_t capacity;
  uint32_t length;
  uint32_t data_size;
  uint32_t flags;
  uint8_t *data;
} *DARRAY;

static uint32_t darray_errno = DARRAY_ERROR_SUCCESS;
static char errmsg[64] = {0};

static uint32_t resize_darray(DARRAY darray);

DARRAY darray_create(uint32_t data_size, uint32_t flags) {
  if (data_size == 0) {
    if (flags & DARRAY_VERBOSE)
      printf("VERBOSE: NULL input.\n");

    darray_errno = DARRAY_ERROR_NULL_INPUT;
    return NULL;
  }

  DARRAY darray = malloc(sizeof(struct darray_s));
  if (!darray) {
    if (flags & DARRAY_VERBOSE)
      printf("VERBOSE: Malloc returned null.\n");

    darray_errno = DARRAY_ERROR_NO_MEM;
    return NULL;
  }

  darray->capacity = 8;
  darray->data = malloc(darray->capacity * data_size);
  if (!darray->data) {
    free(darray);
    if (flags & DARRAY_VERBOSE)
      printf("VERBOSE: Malloc returned null.\n");

    darray_errno = DARRAY_ERROR_NO_MEM;
    return NULL;
  }

  darray->data_size = data_size;
  darray->flags = flags;
  darray->length = 0;

  if (flags & DARRAY_VERBOSE)
    printf("VERBOSE: Dynamic Array created.\n");

  darray_errno = DARRAY_ERROR_SUCCESS;
  return darray;
}

uint32_t darray_destroy(DARRAY *darray, int (*free_func)(void *data)) {
  if (!darray || !*darray) {
    darray_errno = DARRAY_ERROR_NULL_INPUT;
    return 0;
  }

  uint32_t flags = (*darray)->flags;

  if (free_func) {
    void *data;
    for (int i = 0; i < (*darray)->length; i++) {
      darray_get_element((*darray), i, &data);

      if (!free_func(data)) {
        if ((*darray)->flags & DARRAY_VERBOSE) {
          printf("VERBOSE: Provided Freeing Function returned 0.\n");
          printf("VERBOSE: Dyanmic Array not destroyed.\n");
        }

        darray_errno = DARRAY_ERROR_FREE_MEMBER_FAIL;
        return 0;
      }
    }
  }

  free((*darray)->data);
  free(*darray);
  *darray = NULL;

  if (flags & DARRAY_VERBOSE)
    printf("VERBOSE: Dynamic Array destroyed.\n");

  darray_errno = DARRAY_ERROR_SUCCESS;
  return 1;
}

uint32_t darray_clear(DARRAY darray, int (*free_func)(void *data)) {
  if (!darray) {
    darray_errno = DARRAY_ERROR_NULL_INPUT;
    return 0;
  }

  if (free_func) {
    void *data;
    for (int i = 0; i < darray->length; i++) {
      darray_get_element(darray, i, &data);

      if (!free_func(data)) {
        if (darray->flags & DARRAY_VERBOSE) {
          printf("VERBOSE: Provided Freeing Function returned 0.\n");
          printf("VERBOSE: Dyanmic Array not cleared.\n");
        }

        darray_errno = DARRAY_ERROR_FREE_MEMBER_FAIL;
        return 0;
      }
    }
  }

  darray->length = 0;

  if (darray->flags & DARRAY_VERBOSE)
    printf("VERBOSE: Dynamic Array cleared.\n");

  darray_errno = DARRAY_ERROR_SUCCESS;
  return 1;
}

uint32_t darray_append(DARRAY darray, void *data) {
  if (!darray || !data) {
    if (darray && darray->flags & DARRAY_VERBOSE)
      printf("VERBOSE: Null input parameter.\n");

    darray_errno = DARRAY_ERROR_NULL_INPUT;
    return 0;
  }

  if (darray->length == darray->capacity - 1)
    if (!resize_darray(darray))
      return 0;

  void *addr = &darray->data[darray->length * darray->data_size];
  memcpy(addr, data, darray->data_size);

  darray->length++;

  if (darray->flags & DARRAY_VERBOSE)
    printf("VERBOSE: Data appended.\n");

  darray_errno = DARRAY_ERROR_SUCCESS;
  return 1;
}

uint32_t darray_prepend(DARRAY darray, void *data) {
  if (!darray || !data) {
    if (darray && darray->flags & DARRAY_VERBOSE)
      printf("VERBOSE: Null input parameter.\n");

    darray_errno = DARRAY_ERROR_NULL_INPUT;
    return 0;
  }

  if (darray->length == darray->capacity - 1)
    if (!resize_darray(darray))
      return 0;

  memmove(&darray->data[darray->data_size], darray->data,
          darray->length * darray->data_size);
  memcpy(darray->data, data, darray->data_size);

  darray->length++;

  if (darray->flags & DARRAY_VERBOSE)
    printf("VERBOSE: Data prepended.\n");

  darray_errno = DARRAY_ERROR_SUCCESS;
  return 1;
}

uint32_t darray_insert(DARRAY darray, uint32_t index, void *data) {
  if (!darray || !data) {
    if (darray && darray->flags & DARRAY_VERBOSE)
      printf("VERBOSE: Null input parameter.\n");

    darray_errno = DARRAY_ERROR_NULL_INPUT;
    return 0;
  }

  if (index >= darray->length) {
    if (darray->flags & DARRAY_VERBOSE)
      printf("VERBOSE: Index exceeds length.\n");

    darray_errno = DARRAY_ERROR_BAD_INDEX;
    return 0;
  }

  void *src = &darray->data[index * darray->data_size];
  void *dest = &darray->data[(index + 1) * darray->data_size];
  memmove(dest, src, (darray->length - index) * darray->data_size);
  memcpy(src, data, darray->data_size);

  darray->length++;

  if (darray->flags & DARRAY_VERBOSE)
    printf("VERBOSE: Data inserted at index %u.\n", index);

  darray_errno = DARRAY_ERROR_SUCCESS;
  return 1;
}

uint32_t darray_remove(DARRAY darray, uint32_t index,
                       int (*free_func)(void *)) {
  if (!darray) {
    darray_errno = DARRAY_ERROR_NULL_INPUT;
    return 0;
  }

  if (index >= darray->length) {
    if (darray->flags & DARRAY_VERBOSE)
      printf("VERBOSE: Index exceeds length.\n");

    darray_errno = DARRAY_ERROR_BAD_INDEX;
    return 0;
  }

  if (free_func) {
    void *data;
    darray_get_element(darray, index, &data);

    if (!free_func(data)) {
      if (darray->flags & DARRAY_VERBOSE) {
        printf("VERBOSE: Provided Freeing Function returned 0.\n");
        printf("VERBOSE: Index not removed.\n");

        return 0;
      }
    }
  }

  void *src = &darray->data[(index + 1) * darray->data_size];
  void *dest = &darray->data[index * darray->data_size];
  memmove(dest, src, (darray->length - index) * darray->data_size);

  darray->length--;

  if (darray->flags & DARRAY_VERBOSE)
    printf("VERBOSE: Index %u removed.\n", index);

  darray_errno = DARRAY_ERROR_SUCCESS;
  return 1;
}

uint32_t darray_replace(DARRAY darray, uint32_t index, void *data,
                        int (*free_func)(void *data)) {
  if (!darray || !data) {
    if (darray && darray->flags & DARRAY_VERBOSE)
      printf("VERBOSE: Null input parameter.\n");

    darray_errno = DARRAY_ERROR_NULL_INPUT;
    return 0;
  }

  if (index >= darray->length) {
    if (darray->flags & DARRAY_VERBOSE)
      printf("VERBOSE: Index exceeds length.\n");

    darray_errno = DARRAY_ERROR_BAD_INDEX;
    return 0;
  }

  darray_remove(darray, index, free_func);
  darray_insert(darray, index, data);

  if (darray->flags & DARRAY_VERBOSE)
    printf("VERBOSE: Index %u replaced.\n", index);

  darray_errno = DARRAY_ERROR_SUCCESS;
  return 1;
}

uint32_t darray_swap(DARRAY darray, uint32_t index_a, uint32_t index_b) {
  if (!darray) {
    darray_errno = DARRAY_ERROR_NULL_INPUT;
    return 0;
  }

  if (index_a == index_b) {
    darray_errno = DARRAY_ERROR_SUCCESS;
    return 1;
  }

  if (index_a >= darray->length || index_b >= darray->length) {
    if (darray->flags & DARRAY_VERBOSE)
      printf("VERBOSE: Index exceeds length.\n");

    darray_errno = DARRAY_ERROR_BAD_INDEX;
    return 0;
  }

  void *temp = malloc(darray->data_size);
  if (!temp) {
    if (darray->flags & DARRAY_VERBOSE)
      printf("VERBOSE: Malloc returned null.\n");

    darray_errno = DARRAY_ERROR_NO_MEM;
    return 0;
  }

  memcpy(temp, &darray->data[index_a * darray->data_size], darray->data_size);
  memmove(&darray->data[index_a * darray->data_size],
          &darray->data[index_b * darray->data_size], darray->data_size);
  memcpy(&darray->data[index_b * darray->data_size], temp, darray->data_size);
  free(temp);

  if (darray->flags & DARRAY_VERBOSE)
    printf("VERBOSE: Indices %u and %u swapped.\n", index_a, index_b);

  darray_errno = DARRAY_ERROR_SUCCESS;
  return 1;
}

uint32_t darray_sort(DARRAY darray,
                     int (*sort_func)(const void *a, const void *b)) {
  if (!darray || !sort_func) {
    if (darray && darray->flags & DARRAY_VERBOSE)
      printf("VERBOSE: Null input parameter.\n");

    darray_errno = DARRAY_ERROR_NULL_INPUT;
    return 0;
  }

  qsort(darray->data, darray->length, darray->data_size, sort_func);

  if (darray->flags & DARRAY_VERBOSE)
    printf("VERBOSE: Dynamic Array has been sorted.\n");

  darray_errno = DARRAY_ERROR_SUCCESS;
  return 1;
}

uint32_t darray_reverse(DARRAY darray) {
  if (!darray) {
    darray_errno = DARRAY_ERROR_NULL_INPUT;
    return 0;
  }

  for (int i = 0; i < darray->length / 2; i++)
    darray_swap(darray, i, darray->length - i - 1);

  if (darray->flags & DARRAY_VERBOSE)
    printf("VERBOSE: Dynamic Array has been reversed.\n");

  darray_errno = DARRAY_ERROR_SUCCESS;
  return 1;
}

uint32_t darray_get_element(DARRAY darray, uint32_t index, void *output) {
  if (!darray || !output) {
    if (darray && darray->flags & DARRAY_VERBOSE)
      printf("VERBOSE: Null input parameter.\n");

    darray_errno = DARRAY_ERROR_NULL_INPUT;
    return 0;
  }

  if (index >= darray->length) {
    if (darray->flags & DARRAY_VERBOSE)
      printf("VERBOSE: Index exceeds length.\n");

    darray_errno = DARRAY_ERROR_BAD_INDEX;
    return 0;
  }

  memcpy(output, &darray->data[index * darray->data_size], darray->data_size);

  darray_errno = DARRAY_ERROR_SUCCESS;
  return 1;
}

uint32_t darray_get_length(DARRAY darray) {
  if (!darray) {
    darray_errno = DARRAY_ERROR_NULL_INPUT;
    return 0;
  }

  return darray->length;
}

uint32_t darray_get_error() { return darray_errno; }

char *darray_get_error_message() {
  switch (darray_errno) {
  case DARRAY_ERROR_SUCCESS:
    sprintf(errmsg, "Error success");
    break;
  case DARRAY_ERROR_NO_MEM:
    sprintf(errmsg, "No memory");
    break;
  case DARRAY_ERROR_NULL_INPUT:
    sprintf(errmsg, "Null input parameter");
    break;
  case DARRAY_ERROR_BAD_INDEX:
    sprintf(errmsg, "Index exceeds length");
    break;
  case DARRAY_ERROR_FREE_MEMBER_FAIL:
    sprintf(errmsg, "Free Function returned 0");
    break;
  }

  return errmsg;
}

char *darray_get_version() { return VERSION; }

static uint32_t resize_darray(DARRAY darray) {
  uint32_t new_capacity = darray->capacity * 2;
  uint8_t *new_data = malloc(new_capacity * darray->data_size);
  if (!new_data) {
    if (darray->flags & DARRAY_VERBOSE)
      printf("VERBOSE: Failed to reallocate memory.\n");

    darray_errno = DARRAY_ERROR_NO_MEM;
    return 0;
  }

  memmove(new_data, darray->data, darray->capacity * darray->data_size);

  free(darray->data);
  darray->data = new_data;
  darray->capacity *= 2;
  if (darray->flags & DARRAY_VERBOSE)
    printf("VERBOSE: Dyanmic Array reallocated to %u elements.\n",
           new_capacity);

  return 1;
}
