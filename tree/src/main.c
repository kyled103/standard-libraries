#include <stdio.h>
#include <stdlib.h>

#include "../include/tree.h"

int main() {
  TREE tree = tree_create(sizeof(int), 0);

  int a = 1;
  tree_insert_data(tree, NULL, &a);
  a = 2;
  tree_insert_data(tree, NULL, &a);
  a = 3;
  tree_insert_data(tree, NULL, &a);
  a = 4;
  tree_insert_data(tree, NULL, &a);

  TNODE node = tree_get_node(tree, NULL, 2);
  node = tree_get_previous(tree, node);

  int b;
  tree_get_node_data(tree, node, &b);
  printf("%d\n", b);

  tree_clear(tree, NULL);
  tree_destroy(&tree, NULL);
  return 0;
}
