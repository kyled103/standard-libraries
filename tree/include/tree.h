#ifndef TREE_H
#define TREE_H

#include <stdint.h>

// Change Log
// Version  | DATE      | UPDATE
// 1.0        11/10/26   Initial version

// Flags
#define TREE_VERBOSE 0x1

// Error Codes
#define TREE_ERROR_SUCCESS 0x0
#define TREE_ERROR_NO_MEM 0x1
#define TREE_ERROR_NULL_INPUT 0x2
#define TREE_ERROR_BAD_INDEX 0x3

typedef struct tnode_s *TNODE;
typedef struct tree_s *TREE;

TREE tree_create(uint32_t data_size, uint32_t flags);
uint32_t tree_destroy(TREE *tree, int (*free_func)(void *data));
uint32_t tree_clear(TREE tree, int (*free_func)(void *data));

uint32_t tree_insert_data(TREE tree, TNODE parent, void *data);
uint32_t tree_remove_index(TREE tree, TNODE parent, uint32_t index,
                           int (*free_func)(void *data));
uint32_t tree_remove_node(TREE tree, TNODE node, int (*free_func)(void *data));

TNODE tree_get_node(TREE tree, TNODE parent, uint32_t index);
TNODE tree_get_parent(TREE tree, TNODE node);
TNODE tree_get_next(TREE tree, TNODE node);
TNODE tree_get_previous(TREE tree, TNODE node);

uint32_t tree_get_data(TREE tree, TNODE parent, uint32_t index, void *output);
uint32_t tree_get_node_data(TREE tree, TNODE node, void *output);

uint32_t tree_get_child_count(TREE tree, TNODE node);

uint32_t tree_get_error();
char *tree_get_error_message();
char *tree_get_verson();

#endif
