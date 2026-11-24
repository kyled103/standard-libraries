#ifndef QUEUE_H
#define QUEUE_H

#include <stdint.h>

// Change Log
// Version  | DATE    | UPDATE
// 1.0        11/9/26   Initial version

// Flags
#define QUEUE_VERBOSE 0x1

// Error Codes
#define QUEUE_ERROR_SUCCESS 0x0
#define QUEUE_ERROR_NO_MEM 0x1
#define QUEUE_ERROR_NULL_INPUT 0x2
#define QUEUE_ERROR_EMPTY 0x3

typedef struct queue_s *QUEUE;

QUEUE queue_create(uint32_t data_size, uint32_t flags);
uint32_t queue_destroy(QUEUE *queue);
uint32_t queue_clear(QUEUE queue);

uint32_t queue_push(QUEUE queue, void *data);
uint32_t queue_pop(QUEUE queue, void *data);

uint32_t queue_get_error();
char *queue_get_error_message();
char *queue_get_version();

#endif
