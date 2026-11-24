#ifndef LLIST_H
#define LLIST_H

#include <stdint.h>

// Change Log
// Version  | DATE      | UPDATE
// 1.0        11/11/26   Initial version

// Flags
#define LLIST_VERBOSE 0x1
#define LLIST_LOOP 0x2

// Error Codes
#define LLIST_ERROR_SUCCESS 0x0
#define LLIST_ERROR_NO_MEM 0x1
#define LLIST_ERROR_NULL_INPUT 0x2
#define LLIST_ERROR_BAD_INDEX 0x3
#define LLIST_ERROR_FREE_MEMBER_FAIL 0x4

typedef struct lnode_s *LNODE;
typedef struct llist_s *LLIST;

LLIST llist_create(uint32_t data_size, uint32_t flags);
uint32_t llist_destroy(LLIST *llist, int (*free_func)(void *data));
uint32_t llist_clear(LLIST llist, int (*free_func)(void *data));

uint32_t llist_append(LLIST llist, void *data);
uint32_t llist_prepend(LLIST llist, void *data);
uint32_t llist_insert(LLIST llist, uint32_t index, void *data);

uint32_t llist_remove(LLIST llist, uint32_t index,
                      int (*free_func)(void *data));
uint32_t llist_remove_node(LLIST llist, LNODE node,
                           int (*free_func)(void *data));
uint32_t llist_remove_head(LLIST llist, int (*free_func)(void *data));
uint32_t llist_remove_tail(LLIST llist, int (*free_func)(void *data));

uint32_t llist_swap_index(LLIST llist, uint32_t index_a, uint32_t index_b);
uint32_t llist_swap_nodes(LLIST llist, LNODE node_a, LNODE node_b);
uint32_t llist_select_sort(LLIST llist,
                           int32_t (*sort_func)(const void *a, const void *b));
// uint32_t llist_merge_sort(LLIST llist,
//                           int32_t (*sort_func)(const void *a, const void
//                           *b));
uint32_t llist_reverse(LLIST llist);

uint32_t llist_get_data(LLIST llist, uint32_t index, void *output);
uint32_t llist_get_node_data(LLIST llist, LNODE node, void *output);
uint32_t llist_get_head_data(LLIST llist, void *output);
uint32_t llist_get_tail_data(LLIST llist, void *output);

LNODE llist_get_node(LLIST llist, uint32_t index);
LNODE llist_get_head(LLIST llist);
LNODE llist_get_tail(LLIST llist);

LNODE llist_get_next(LNODE node);
LNODE llist_get_prev(LNODE node);

uint32_t llist_get_length(LLIST llist);
uint32_t llist_get_error();
char *llist_get_error_message();
char *llist_get_version();

#endif
