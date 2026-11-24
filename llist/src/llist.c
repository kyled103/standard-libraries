#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/llist.h"

#define VERSION "1.0"

typedef struct lnode_s {
  void *data;
  struct lnode_s *prev;
  struct lnode_s *next;
} *LNODE;

typedef struct llist_s {
  uint32_t length;
  uint32_t data_size;
  uint32_t flags;
  LNODE head;
  LNODE tail;
} *LLIST;

static uint32_t llist_errno = LLIST_ERROR_SUCCESS;
static char errmsg[64] = {0};

static LNODE create_node(LLIST llist, void *data);
static uint32_t free_node(LLIST llist, LNODE node,
                          int (*free_func)(void *data));

LLIST llist_create(uint32_t data_size, uint32_t flags) {
  if (data_size == 0) {
    if (flags & LLIST_VERBOSE)
      printf("VERBOSE: NULL input.\n");

    llist_errno = LLIST_ERROR_NULL_INPUT;
    return NULL;
  }

  LLIST llist = malloc(sizeof(struct llist_s));
  if (!llist) {
    if (flags & LLIST_VERBOSE)
      printf("VERBOSE: Malloc returned null.\n");

    llist_errno = LLIST_ERROR_NO_MEM;
    return NULL;
  }

  llist->length = 0;
  llist->data_size = data_size;
  llist->flags = flags;
  llist->head = NULL;
  llist->tail = NULL;

  if (flags & LLIST_VERBOSE)
    printf("VERBOSE: Linked List created.\n");

  llist_errno = LLIST_ERROR_SUCCESS;
  return llist;
}

uint32_t llist_destroy(LLIST *llist, int (*free_func)(void *data)) {
  if (!llist || !*llist) {
    llist_errno = LLIST_ERROR_NULL_INPUT;
    return 0;
  }

  uint32_t flags = (*llist)->flags;
  LNODE node = (*llist)->tail;

  while (node) {
    llist_remove_node(*llist, node, free_func);
    node = (*llist)->tail;
  }

  free(*llist);
  *llist = NULL;

  if (flags & LLIST_VERBOSE)
    printf("VERBOSE: Linked List destroyed.\n");

  llist_errno = LLIST_ERROR_SUCCESS;
  return 1;
}

uint32_t llist_clear(LLIST llist, int (*free_func)(void *data)) {
  if (!llist) {
    llist_errno = LLIST_ERROR_NULL_INPUT;
    return 0;
  }

  uint32_t flags = llist->flags;
  LNODE node = llist->tail;

  while (node) {
    llist_remove_node(llist, node, free_func);
    node = llist->tail;
  }

  if (flags & LLIST_VERBOSE)
    printf("VERBOSE: Linked List cleared.\n");

  llist_errno = LLIST_ERROR_SUCCESS;
  return 1;
}

uint32_t llist_append(LLIST llist, void *data) {
  if (!llist || !data) {
    if (llist && llist->flags & LLIST_VERBOSE)
      printf("VERBOSE: Null input parameter.\n");

    llist_errno = LLIST_ERROR_NULL_INPUT;
    return 0;
  }

  LNODE new_node = create_node(llist, data);
  if (!new_node)
    return 0;

  if (llist->tail) {
    llist->tail->next = new_node;
    new_node->prev = llist->tail;

    if (llist->flags & LLIST_LOOP)
      new_node->next = llist->head;

    llist->tail = new_node;

    if (llist->flags & LLIST_LOOP)
      llist->head->prev = llist->tail;
  } else {
    if (llist->flags & LLIST_LOOP) {
      new_node->next = new_node;
      new_node->prev = new_node;
    }

    llist->head = new_node;
    llist->tail = new_node;
  }

  llist->length++;

  if (llist->flags & LLIST_VERBOSE)
    printf("VERBOSE: Node appended.\n");

  llist_errno = LLIST_ERROR_SUCCESS;
  return 1;
}

uint32_t llist_prepend(LLIST llist, void *data) {
  if (!llist || !data) {
    if (llist && llist->flags & LLIST_VERBOSE)
      printf("VERBOSE: Null input parameter.\n");

    llist_errno = LLIST_ERROR_NULL_INPUT;
    return 0;
  }

  LNODE new_node = create_node(llist, data);
  if (!new_node)
    return 0;

  if (llist->head) {
    llist->head->prev = new_node;
    new_node->next = llist->head;
  }

  if (llist->flags & LLIST_LOOP) {
    new_node->prev = llist->tail;
    llist->tail->next = new_node;
  }

  llist->head = new_node;
  llist->length++;

  if (llist->flags & LLIST_VERBOSE)
    printf("VERBOSE: Node prepended.\n");

  llist_errno = LLIST_ERROR_SUCCESS;
  return 1;
}

uint32_t llist_insert(LLIST llist, uint32_t index, void *data) {
  if (!llist || !data) {
    if (llist && llist->flags & LLIST_VERBOSE)
      printf("VERBOSE: Null input parameter.\n");

    llist_errno = LLIST_ERROR_NULL_INPUT;
    return 0;
  }

  if (index > llist->length) {
    if (llist->flags & LLIST_VERBOSE)
      printf("VERBOSE: Index exceeds length.\n");

    llist_errno = LLIST_ERROR_BAD_INDEX;
    return 0;
  }

  if (index == 0)
    return llist_prepend(llist, data);
  if (index == llist->length)
    return llist_append(llist, data);

  LNODE new_node = create_node(llist, data);
  if (!new_node)
    return 0;

  LNODE prev = llist_get_node(llist, index - 1);
  LNODE next = prev->next;

  new_node->prev = prev;
  new_node->next = next;

  prev->next = new_node;
  next->prev = new_node;

  llist->length++;

  if (llist->flags & LLIST_VERBOSE)
    printf("VERBOSE: Node inserted.\n");

  llist_errno = LLIST_ERROR_SUCCESS;
  return 1;
}

uint32_t llist_remove(LLIST llist, uint32_t index,
                      int (*free_func)(void *data)) {
  if (!llist) {
    llist_errno = LLIST_ERROR_NULL_INPUT;
    return 0;
  }

  if (index >= llist->length) {
    if (llist->flags & LLIST_VERBOSE)
      printf("VERBOSE: Index exceeds length.\n");

    llist_errno = LLIST_ERROR_BAD_INDEX;
    return 0;
  }

  LNODE node = llist->head;
  for (int i = 0; i < index; i++)
    node = node->next;

  return llist_remove_node(llist, node, free_func);
}

uint32_t llist_remove_node(LLIST llist, LNODE node,
                           int (*free_func)(void *data)) {
  if (!llist || !node) {
    if (llist && llist->flags & LLIST_VERBOSE)
      printf("VERBOSE: Null input parameter.\n");

    llist_errno = LLIST_ERROR_NULL_INPUT;
    return 0;
  }
  uint8_t is_head = node == llist->head;
  uint8_t is_tail = node == llist->tail;

  LNODE prev_node = node->prev;
  LNODE next_node = node->next;

  if (!free_node(llist, node, free_func))
    return 0;

  if (is_head)
    llist->head = next_node;
  if (is_tail)
    llist->tail = prev_node;

  if (prev_node)
    prev_node->next = next_node;
  if (next_node)
    next_node->prev = prev_node;

  llist->length--;

  if (llist->length == 0) {
    llist->head = NULL;
    llist->tail = NULL;
  }

  if (llist->flags & LLIST_VERBOSE)
    printf("VERBOSE: Node removed.\n");

  llist_errno = LLIST_ERROR_SUCCESS;
  return 1;
}

uint32_t llist_remove_head(LLIST llist, int (*free_func)(void *data)) {
  if (!llist) {
    llist_errno = LLIST_ERROR_NULL_INPUT;
    return 0;
  }

  return llist_remove_node(llist, llist->head, free_func);
}

uint32_t llist_remove_tail(LLIST llist, int (*free_func)(void *data)) {
  if (!llist) {
    llist_errno = LLIST_ERROR_NULL_INPUT;
    return 0;
  }

  return llist_remove_node(llist, llist->tail, free_func);
}

uint32_t llist_swap_index(LLIST llist, uint32_t index_a, uint32_t index_b) {
  if (!llist) {
    llist_errno = LLIST_ERROR_NULL_INPUT;
    return 0;
  }

  if (index_a >= llist->length && index_b >= llist->length) {
    if (llist->flags & LLIST_VERBOSE)
      printf("VERBOSE: Index exceeds length.\n");

    llist_errno = LLIST_ERROR_BAD_INDEX;
    return 0;
  }

  if (index_a == index_b) {
    if (llist->flags & LLIST_VERBOSE)
      printf("VERBOSE: Indicies indentical.\n");

    llist_errno = LLIST_ERROR_SUCCESS;
    return 1;
  }

  LNODE node_a = llist_get_node(llist, index_a);
  LNODE node_b = llist_get_node(llist, index_b);

  return llist_swap_nodes(llist, node_a, node_b);
}

uint32_t llist_swap_nodes(LLIST llist, LNODE node_a, LNODE node_b) {
  if (!llist || !node_a || !node_b) {
    if (llist && llist->flags & LLIST_VERBOSE)
      printf("VERBOSE: Null input parameter.\n");

    llist_errno = LLIST_ERROR_NULL_INPUT;
    return 0;
  }

  uint8_t are_adjacent = 0;
  if (node_a->next == node_b)
    are_adjacent = 1;
  if (node_b->next == node_a) {
    LNODE temp = node_a;
    node_a = node_b;
    node_b = temp;

    are_adjacent = 1;
  }

  if (are_adjacent) {
    LNODE prev_a = node_a->prev;
    LNODE next_b = node_b->next;

    node_a->prev = node_b;
    node_a->next = next_b;

    node_b->prev = prev_a;
    node_b->next = node_a;

    if (prev_a)
      prev_a->next = node_b;
    if (next_b)
      next_b->prev = node_a;
  } else {
    LNODE prev_a = node_a->prev;
    LNODE next_a = node_a->next;

    LNODE prev_b = node_b->prev;
    LNODE next_b = node_b->next;

    node_a->prev = prev_b;
    node_a->next = next_b;

    node_b->prev = prev_a;
    node_b->next = next_a;

    if (prev_a)
      prev_a->next = node_b;
    if (next_a)
      next_a->prev = node_b;

    if (prev_b)
      prev_b->next = node_a;
    if (next_b)
      next_b->prev = node_a;
  }

  if (llist->head == node_a)
    llist->head = node_b;
  else if (llist->head == node_b)
    llist->head = node_a;

  if (llist->tail == node_a)
    llist->tail = node_b;
  else if (llist->tail == node_b)
    llist->tail = node_a;

  if (llist->flags & LLIST_VERBOSE)
    printf("VERBOSE: Nodes swapped.\n");

  llist_errno = LLIST_ERROR_SUCCESS;
  return 1;
}

uint32_t llist_select_sort(LLIST llist,
                           int32_t (*sort_func)(const void *a, const void *b)) {
  if (!llist || !sort_func) {
    if (llist && llist->flags & LLIST_VERBOSE)
      printf("VERBOSE: Null input parameter.\n");

    llist_errno = LLIST_ERROR_NULL_INPUT;
    return 0;
  }

  if (llist->length < 2) {
    if (llist->flags & LLIST_VERBOSE)
      printf("VERBOSE: Linked List already sorted.\n");

    llist_errno = LLIST_ERROR_SUCCESS;
    return 1;
  }

  for (int i = 0; i < llist->length - 2; i++) {
    for (int j = i + 1; j < llist->length - 1; j++) {
      LNODE a = llist_get_node(llist, i);
      LNODE b = llist_get_node(llist, j);

      if (sort_func(a->data, b->data) > 0)
        llist_swap_nodes(llist, a, b);
    }
  }

  if (llist->flags & LLIST_VERBOSE)
    printf("VERBOSE: Linked List sorted.\n");

  llist_errno = LLIST_ERROR_SUCCESS;
  return 1;
}

uint32_t llist_merge_sort(LLIST llist,
                          int32_t (*sort_func)(const void *a, const void *b)) {
// TODO: Implement
#warning llist_merge_sort is not yet implemented
  return 1;
}

uint32_t llist_reverse(LLIST llist) {
  if (!llist) {
    llist_errno = LLIST_ERROR_NULL_INPUT;
    return 0;
  }

  for (int i = 0; i < llist->length / 2; i++)
    llist_swap_index(llist, i, llist->length - i - 1);

  if (llist->flags & LLIST_VERBOSE)
    printf("VERBOSE: Linked List reversed.\n");

  llist_errno = LLIST_ERROR_SUCCESS;
  return 1;
}

uint32_t llist_get_data(LLIST llist, uint32_t index, void *output) {
  if (!llist || !output) {
    if (llist && llist->flags & LLIST_VERBOSE)
      printf("VERBOSE: Null input parameter.\n");

    llist_errno = LLIST_ERROR_NULL_INPUT;
    return 0;
  }

  if (index >= llist->length) {
    if (llist->flags & LLIST_VERBOSE)
      printf("VERBOSE: Index exceeds length.\n");

    llist_errno = LLIST_ERROR_BAD_INDEX;
    return 0;
  }

  LNODE node = llist_get_node(llist, index);
  memcpy(output, node->data, llist->data_size);

  llist_errno = LLIST_ERROR_SUCCESS;
  return 1;
}

uint32_t llist_get_node_data(LLIST llist, LNODE node, void *output) {
  if (!llist || !node || !output) {
    if (llist && llist->flags & LLIST_VERBOSE)
      printf("VERBOSE: Null input parameter.\n");

    llist_errno = LLIST_ERROR_NULL_INPUT;
    return 0;
  }

  memcpy(output, node->data, llist->data_size);

  llist_errno = LLIST_ERROR_SUCCESS;
  return 1;
}

uint32_t llist_get_head_data(LLIST llist, void *output) {
  if (!llist || !output) {
    if (llist && llist->flags & LLIST_VERBOSE)
      printf("VERBOSE: Null input parameter.\n");

    llist_errno = LLIST_ERROR_NULL_INPUT;
    return 0;
  }

  return llist_get_node_data(llist, llist->head, output);
}

uint32_t llist_get_tail_data(LLIST llist, void *output) {
  if (!llist || !output) {
    if (llist && llist->flags & LLIST_VERBOSE)
      printf("VERBOSE: Null input parameter.\n");

    llist_errno = LLIST_ERROR_NULL_INPUT;
    return 0;
  }

  return llist_get_node_data(llist, llist->tail, output);
}

LNODE llist_get_node(LLIST llist, uint32_t index) {
  if (!llist) {
    llist_errno = LLIST_ERROR_NULL_INPUT;
    return 0;
  }

  if (index >= llist->length) {
    if (llist->flags & LLIST_VERBOSE)
      printf("VERBOSE: Index exceeds length.\n");

    llist_errno = LLIST_ERROR_BAD_INDEX;
    return 0;
  }

  LNODE node;
  if (index < llist->length / 2) {
    node = llist->head;

    for (int i = 0; i < index; i++)
      node = node->next;
  } else {
    node = llist->tail;

    for (int i = 0; i < llist->length - index - 1; i++)
      node = node->prev;
  }

  llist_errno = LLIST_ERROR_SUCCESS;
  return node;
}

LNODE llist_get_head(LLIST llist) {
  if (!llist) {
    llist_errno = LLIST_ERROR_NULL_INPUT;
    return 0;
  }

  llist_errno = LLIST_ERROR_SUCCESS;
  return llist->head;
}

LNODE llist_get_tail(LLIST llist) {
  if (!llist) {
    llist_errno = LLIST_ERROR_NULL_INPUT;
    return 0;
  }

  llist_errno = LLIST_ERROR_SUCCESS;
  return llist->tail;
}

LNODE llist_get_next(LNODE node) {
  if (node) {
    llist_errno = LLIST_ERROR_SUCCESS;
    return node->next;
  }

  llist_errno = LLIST_ERROR_NULL_INPUT;
  return NULL;
}

LNODE llist_get_prev(LNODE node) {
  if (node) {
    llist_errno = LLIST_ERROR_SUCCESS;
    return node->prev;
  }

  llist_errno = LLIST_ERROR_NULL_INPUT;
  return NULL;
}

uint32_t llist_get_length(LLIST llist) {
  if (!llist) {
    llist_errno = LLIST_ERROR_NULL_INPUT;
    return 0;
  }

  return llist->length;
}

uint32_t llist_get_error() { return llist_errno; }

char *llist_get_error_message() {
  switch (llist_errno) {
  case LLIST_ERROR_SUCCESS:
    sprintf(errmsg, "Error success");
    break;
  case LLIST_ERROR_NO_MEM:
    sprintf(errmsg, "No memory");
    break;
  case LLIST_ERROR_NULL_INPUT:
    sprintf(errmsg, "Null input parameter");
    break;
  case LLIST_ERROR_BAD_INDEX:
    sprintf(errmsg, "Index exceeds length");
    break;
  case LLIST_ERROR_FREE_MEMBER_FAIL:
    sprintf(errmsg, "Free Function returned 0");
    break;
  }

  return errmsg;
}

char *llist_get_version() { return VERSION; }

static LNODE create_node(LLIST llist, void *data) {
  LNODE node = malloc(sizeof(struct lnode_s));
  if (!node) {
    if (llist->flags & LLIST_VERBOSE)
      printf("VERBOSE: Malloc returned null.\n");

    llist_errno = LLIST_ERROR_NO_MEM;
    return NULL;
  }

  node->data = malloc(llist->data_size);
  if (!node->data) {
    if (llist->flags & LLIST_VERBOSE)
      printf("VERBOSE: Malloc returned null.\n");

    free(node);
    llist_errno = LLIST_ERROR_NO_MEM;
    return NULL;
  }

  memcpy(node->data, data, llist->data_size);
  node->prev = NULL;
  node->next = NULL;

  llist_errno = LLIST_ERROR_SUCCESS;
  return node;
}

static uint32_t free_node(LLIST llist, LNODE node,
                          int (*free_func)(void *data)) {
  if (free_func) {
    if (!free_func(node->data)) {
      if (llist->flags & LLIST_VERBOSE) {
        printf("VERBOSE: Provided Freeing Function returned 0.\n");
        printf("VERBOSE: Dyanmic Array not destroyed.\n");
      }

      llist_errno = LLIST_ERROR_FREE_MEMBER_FAIL;
      return 0;
    }
  }

  free(node->data);
  free(node);

  return 1;
}
