#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/queue.h"

#define VERSION "1.0"

typedef struct queue_s {
  uint32_t capacity;
  uint32_t length;
  uint32_t data_size;
  uint32_t flags;
  uint8_t *data;
} *QUEUE;

static uint32_t queue_errno = QUEUE_ERROR_SUCCESS;
static char errmsg[64] = {0};

static uint32_t resize_queue(QUEUE queue);

QUEUE queue_create(uint32_t data_size, uint32_t flags) {
  if (data_size == 0) {
    if (flags & QUEUE_VERBOSE)
      printf("VERBOSE: NULL input.\n");

    queue_errno = QUEUE_ERROR_NULL_INPUT;
    return NULL;
  }

  QUEUE queue = malloc(sizeof(struct queue_s));
  if (!queue) {
    if (flags & QUEUE_VERBOSE)
      printf("VERBOSE: Malloc returned null.\n");

    queue_errno = QUEUE_ERROR_NO_MEM;
    return NULL;
  }

  queue->capacity = 8;
  queue->data = calloc(queue->capacity, data_size);
  if (!queue->data) {
    free(queue);
    if (flags & QUEUE_VERBOSE)
      printf("VERBOSE: Malloc returned null.\n");

    queue_errno = QUEUE_ERROR_NO_MEM;
    return NULL;
  }

  queue->data_size = data_size;
  queue->length = 0;
  queue->flags = flags;

  if (flags & QUEUE_VERBOSE)
    printf("VERBOSE: Queue created.\n");

  queue_errno = QUEUE_ERROR_SUCCESS;
  return queue;
}

uint32_t queue_destroy(QUEUE *queue) {
  if (!queue || !*queue) {
    queue_errno = QUEUE_ERROR_NULL_INPUT;
    return 0;
  }
  uint32_t flags = (*queue)->flags;

  if ((*queue)->flags & QUEUE_VERBOSE)
    printf("VERBOSE: Stack destroyed.\n");

  free((*queue)->data);
  free(*queue);
  *queue = NULL;

  if (flags & QUEUE_VERBOSE)
    printf("VERBOSE: Stack destroyed.\n");

  queue_errno = QUEUE_ERROR_SUCCESS;
  return 1;
}

uint32_t queue_clear(QUEUE queue) {
  if (!queue) {
    if (queue && queue->flags & QUEUE_VERBOSE)
      printf("VERBOSE: Null input parameter.\n");

    queue_errno = QUEUE_ERROR_NULL_INPUT;
    return 0;
  }

  queue->length = 0;

  if (queue->flags & QUEUE_VERBOSE)
    printf("VERBOSE: Queue cleared.\n");

  queue_errno = QUEUE_ERROR_SUCCESS;
  return 1;
}

uint32_t queue_push(QUEUE queue, void *data) {
  if (!queue || !data) {
    if (queue && queue->flags & QUEUE_VERBOSE)
      printf("VERBOSE: Null input parameter.\n");

    queue_errno = QUEUE_ERROR_NULL_INPUT;
    return 0;
  }

  if (queue->length == queue->capacity) {
    if (!resize_queue(queue))
      return 0;
  }

  void *addr = &queue->data[queue->length++ * queue->data_size];
  memcpy(addr, data, queue->data_size);

  if (queue->flags & QUEUE_VERBOSE)
    printf("VERBOSE: Queue pushed.\n");

  queue_errno = QUEUE_ERROR_SUCCESS;
  return 1;
}

uint32_t queue_pop(QUEUE queue, void *data) {
  if (!queue || !data) {
    if (queue && queue->flags & QUEUE_VERBOSE)
      printf("VERBOSE: Null input parameter.\n");

    queue_errno = QUEUE_ERROR_NULL_INPUT;
    return 0;
  }

  if (queue->length == 0) {
    if (queue->flags & QUEUE_VERBOSE)
      printf("VERBOSE: Queue is empty.\n");

    queue_errno = QUEUE_ERROR_EMPTY;
    return 0;
  }

  uint32_t length = ((queue->length - 1) * queue->data_size);
  memcpy(data, queue->data, queue->data_size);
  memmove(queue->data, &queue->data[queue->data_size], length);
  queue->length--;

  if (queue->flags & QUEUE_VERBOSE)
    printf("VERBOSE: Queue popped.\n");

  queue_errno = QUEUE_ERROR_SUCCESS;
  return 1;
}

uint32_t queue_get_error() { return queue_errno; }

char *queue_get_error_message() {
  switch (queue_errno) {
  case QUEUE_ERROR_SUCCESS:
    sprintf(errmsg, "Error success");
    break;
  case QUEUE_ERROR_NO_MEM:
    sprintf(errmsg, "No memory");
    break;
  case QUEUE_ERROR_NULL_INPUT:
    sprintf(errmsg, "Null input parameter");
    break;
  case QUEUE_ERROR_EMPTY:
    sprintf(errmsg, "Queue is empty");
    break;
  }

  return errmsg;
}

char *queue_get_version() { return VERSION; }

static uint32_t resize_queue(QUEUE queue) {
  uint32_t new_capacity = queue->capacity * 2;
  uint8_t *new_data = calloc(new_capacity, queue->data_size);
  if (!new_data) {
    if (queue->flags & QUEUE_VERBOSE)
      printf("VERBOSE: Failed to reallocate memory.\n");

    queue_errno = QUEUE_ERROR_NO_MEM;
    return 0;
  }

  memmove(new_data, queue->data, queue->capacity * queue->data_size);

  free(queue->data);
  queue->data = new_data;
  queue->capacity *= 2;
  if (queue->flags & QUEUE_VERBOSE)
    printf("VERBOSE: queue reallocated to %u elements.\n", new_capacity);

  return 1;
}
