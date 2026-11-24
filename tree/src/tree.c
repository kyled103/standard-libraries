#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../build/include/darray.h"
#include "../include/tree.h"

#define VERSION "1.0"

typedef struct tnode_s {
  void *data;
  DARRAY children;
  struct tnode_s *parent;
} *TNODE;

typedef struct tree_s {
  uint32_t data_size;
  uint32_t flags;
  DARRAY head;
} *TREE;

static uint32_t tree_errno = TREE_ERROR_SUCCESS;
static char errmsg[64] = {0};

static TNODE create_node(TREE tree, TNODE parent, void *data);
static uint32_t remove_node(TREE tree, TNODE node,
                            int (*free_func)(void *data));

TREE tree_create(uint32_t data_size, uint32_t flags) {
  if (data_size == 0) {
    if (flags & TREE_VERBOSE)
      printf("VERBOSE: NULL input.\n");

    tree_errno = TREE_ERROR_NULL_INPUT;
    return NULL;
  }

  TREE tree = malloc(sizeof(struct tree_s));
  if (!tree) {
    if (flags & TREE_VERBOSE)
      printf("VERBOSE: Malloc returned null.\n");

    tree_errno = TREE_ERROR_NO_MEM;
    return NULL;
  }

  tree->head = darray_create(sizeof(TNODE), 0);
  if (!tree->head) {
    if (flags & TREE_VERBOSE)
      printf("VERBOSE: Error creating head list.\n");

    free(tree);
    tree_errno = TREE_ERROR_NO_MEM;
    return NULL;
  }

  tree->data_size = data_size;
  tree->flags = flags;

  if (flags & TREE_VERBOSE)
    printf("VERBOSE: Tree created.\n");

  tree_errno = TREE_ERROR_SUCCESS;
  return tree;
}

uint32_t tree_destroy(TREE *tree, int (*free_func)(void *data)) {
  if (!tree || !*tree)
    return 0;

  uint32_t flags = (*tree)->flags;

  tree_clear(*tree, free_func);
  darray_destroy(&(*tree)->head, 0);
  free(*tree);
  *tree = NULL;

  if (flags & TREE_VERBOSE)
    printf("VERBOSE: Tree destroyed.\n");

  tree_errno = TREE_ERROR_SUCCESS;
  return 1;
}

uint32_t tree_clear(TREE tree, int (*free_func)(void *data)) {
  if (!tree) {
    tree_errno = TREE_ERROR_NULL_INPUT;
    return 0;
  }

  uint32_t length = darray_get_length(tree->head);
  if (length == 0) {
    tree_errno = TREE_ERROR_SUCCESS;
    return 1;
  }

  TNODE node;
  for (int i = length - 1; i >= 0; i--) {
    darray_get_element(tree->head, i, &node);
    remove_node(tree, node, free_func);
  }

  darray_clear(tree->head, free_func);

  if (tree->flags & TREE_VERBOSE)
    printf("VERBOSE: Tree cleared.\n");

  tree_errno = TREE_ERROR_SUCCESS;
  return 1;
}

uint32_t tree_insert_data(TREE tree, TNODE parent, void *data) {
  if (!tree || !data) {
    if (tree && tree->flags & TREE_VERBOSE)
      printf("VERBOSE: NULL input.\n");

    tree_errno = TREE_ERROR_NULL_INPUT;
    return 0;
  }

  TNODE node = create_node(tree, parent, data);
  if (!node)
    return 0;

  if (parent)
    darray_append(parent->children, &node);
  else
    darray_append(tree->head, &node);

  tree_errno = TREE_ERROR_SUCCESS;
  return 1;
}

uint32_t tree_remove_index(TREE tree, TNODE parent, uint32_t index,
                           int (*free_func)(void *data)) {
  if (!tree) {
    tree_errno = TREE_ERROR_NULL_INPUT;
    return 1;
  }

  DARRAY list = NULL;
  if (parent)
    list = parent->children;
  else
    list = tree->head;

  if (index >= darray_get_length(list)) {
    if (tree->flags & TREE_VERBOSE)
      printf("VERBOSE: Index exceeds node limit.\n");

    tree_errno = TREE_ERROR_BAD_INDEX;
    return 0;
  }

  TNODE node;
  darray_get_element(list, index, &node);

  return tree_remove_node(tree, node, free_func);
}

uint32_t tree_remove_node(TREE tree, TNODE node, int (*free_func)(void *data)) {
  if (!tree || !node) {
    if (tree && tree->flags & TREE_VERBOSE)
      printf("VERBOSE: NULL input.\n");

    tree_errno = TREE_ERROR_NULL_INPUT;
    return 1;
  }

  DARRAY list = NULL;
  if (node->parent)
    list = node->parent->children;
  else
    list = tree->head;

  uint32_t index = 0;
  TNODE current;
  for (; index < darray_get_length(list); index++) {
    darray_get_element(list, index, &current);
    if (current == node)
      break;
  }

  remove_node(tree, node, free_func);
  darray_remove(list, index, NULL);

  if (tree->flags & TREE_VERBOSE)
    printf("VERBOSE: Node removed.\n");

  tree_errno = TREE_ERROR_SUCCESS;
  return 1;
}

TNODE tree_get_node(TREE tree, TNODE parent, uint32_t index) {
  if (!tree) {
    if (tree && tree->flags & TREE_VERBOSE)
      printf("VERBOSE: NULL input.\n");

    tree_errno = TREE_ERROR_NULL_INPUT;
    return NULL;
  }

  DARRAY list = NULL;
  TNODE node;
  if (parent) {
    if (index >= darray_get_length(parent->children)) {
      if (tree->flags & TREE_VERBOSE)
        printf("VERBOSE: Index exceeds node limit.\n");

      tree_errno = TREE_ERROR_BAD_INDEX;
      return NULL;
    }

    list = parent->children;
  } else {
    if (index >= darray_get_length(tree->head)) {
      if (tree->flags & TREE_VERBOSE)
        printf("VERBOSE: Index exceeds node limit.\n");

      tree_errno = TREE_ERROR_BAD_INDEX;
      return NULL;
    }

    list = tree->head;
  }

  darray_get_element(list, index, &node);

  tree_errno = TREE_ERROR_SUCCESS;
  return node;
}

TNODE tree_get_parent(TREE tree, TNODE node) {
  if (!tree || !node) {
    if (tree->flags & TREE_VERBOSE)
      printf("VERBOSE: NULL input.\n");

    tree_errno = TREE_ERROR_NULL_INPUT;
    return NULL;
  }

  tree_errno = TREE_ERROR_SUCCESS;
  return node->parent;
}

TNODE tree_get_next(TREE tree, TNODE node) {
  if (!tree || !node) {
    if (tree && tree->flags & TREE_VERBOSE)
      printf("VERBOSE: NULL input.\n");

    tree_errno = TREE_ERROR_NULL_INPUT;
    return NULL;
  }

  TNODE next = NULL;
  DARRAY list = NULL;
  if (node->parent) {
    list = node->parent->children;
  } else {
    list = tree->head;
  }

  TNODE temp;
  for (int i = 0; i < darray_get_length(list); i++) {
    darray_get_element(list, i, &temp);
    if (temp == node) {
      darray_get_element(list, i + 1, &next);
      break;
    }
  }

  tree_errno = TREE_ERROR_SUCCESS;
  return next;
}

TNODE tree_get_previous(TREE tree, TNODE node) {
  if (!tree || !node) {
    if (tree && tree->flags & TREE_VERBOSE)
      printf("VERBOSE: NULL input.\n");

    tree_errno = TREE_ERROR_NULL_INPUT;
    return NULL;
  }

  TNODE prev;
  DARRAY list = NULL;
  if (node->parent) {
    list = node->parent->children;
  } else {
    list = tree->head;
  }

  TNODE temp;
  for (int i = 0; i < darray_get_length(list); i++) {
    darray_get_element(list, i, &temp);
    if (temp == node) {
      darray_get_element(list, i - 1, &prev);
      break;
    }
  }

  tree_errno = TREE_ERROR_SUCCESS;
  return prev;
}

uint32_t tree_get_data(TREE tree, TNODE parent, uint32_t index, void *output) {
  TNODE node = tree_get_node(tree, parent, index);
  if (!node)
    return 0;

  return tree_get_node_data(tree, node, output);
}

uint32_t tree_get_node_data(TREE tree, TNODE node, void *output) {
  if (!tree || !node || !output) {
    if (tree && tree->flags & TREE_VERBOSE)
      printf("VERBOSE: NULL input.\n");

    tree_errno = TREE_ERROR_NULL_INPUT;
    return 0;
  }

  memcpy(output, node->data, tree->data_size);

  tree_errno = TREE_ERROR_SUCCESS;
  return 1;
}

uint32_t tree_get_child_count(TREE tree, TNODE node) {
  if (!tree) {
    tree_errno = TREE_ERROR_NULL_INPUT;
    return 0;
  }

  tree_errno = TREE_ERROR_SUCCESS;
  if (node)
    return darray_get_length(node->children);

  return darray_get_length(tree->head);
}

uint32_t tree_get_error() { return tree_errno; }

char *tree_get_error_message() {
  switch (tree_errno) {
  case TREE_ERROR_SUCCESS:
    sprintf(errmsg, "Error success");
    break;
  case TREE_ERROR_NO_MEM:
    sprintf(errmsg, "No memory");
    break;
  case TREE_ERROR_NULL_INPUT:
    sprintf(errmsg, "Null input parameter");
    break;
  case TREE_ERROR_BAD_INDEX:
    sprintf(errmsg, "Index exceeds length");
    break;
  }

  return errmsg;
}

char *tree_get_verson() { return VERSION; }

static TNODE create_node(TREE tree, TNODE parent, void *data) {
  TNODE node = malloc(sizeof(struct tnode_s));
  if (!node) {
    if (tree->flags & TREE_VERBOSE)
      printf("VERBOSE: Malloc returned null.\n");

    tree_errno = TREE_ERROR_NO_MEM;
    return NULL;
  }

  node->data = malloc(tree->data_size);
  if (!node->data) {
    if (tree->flags & TREE_VERBOSE)
      printf("VERBOSE: Malloc returned null.\n");

    free(node);

    tree_errno = TREE_ERROR_NO_MEM;
    return NULL;
  }

  node->children = darray_create(sizeof(TNODE), 0);
  if (!node->children) {
    if (tree->flags & TREE_VERBOSE)
      printf("VERBOSE: Error creating child list.\n");

    free(node->data);
    free(node);

    tree_errno = TREE_ERROR_NO_MEM;
    return NULL;
  }

  memcpy(node->data, data, tree->data_size);
  node->parent = parent;

  if (tree->flags & TREE_VERBOSE)
    printf("VERBOSE: Node created.\n");

  tree_errno = TREE_ERROR_SUCCESS;
  return node;
}

static uint32_t remove_node(TREE tree, TNODE node,
                            int (*free_func)(void *data)) {
  uint32_t length = darray_get_length(node->children);
  if (length != 0) {
    TNODE child;
    for (int i = length; i >= 0; i--) {
      darray_get_element(node->children, i, &child);
      remove_node(tree, child, free_func);
    }
  }

  if (free_func)
    free_func(node->data);
  darray_destroy(&node->children, 0);
  free(node->data);
  free(node);

  if (tree->flags & TREE_VERBOSE)
    printf("VERBOSE: Node destroyed.\n");

  tree_errno = TREE_ERROR_SUCCESS;
  return 1;
}
