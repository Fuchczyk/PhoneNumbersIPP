/**
 * @file blackred_tree.c
 * @author Przemysław Fuchs (pf438429@students.mimuw.edu.pl)
 * @brief Module implements blackred_tree.h interace. Tree does not have NULL
 * children, they are signalized by guard-children.
 * @date 2022-05-22
 */
#include "blackred_tree.h"
#include "memory.h"
#include "string_lib.h"
#include <string.h>

/**
 * @brief Struct to represent node of Black-Red Tree.
 */
struct BRNode {
  struct BRNode *father;      ///< Pointer to father of node.
  struct BRNode *left_child;  ///< Pointer to left child of node.
  struct BRNode *right_child; ///< Pointer to right child of node.
  bool is_black;              ///< True if colour of given node is black.

  char *value; ///< Pointer to node's value.
};

/**
 * @brief Typedef to keep code clean and more readable.
 */
typedef struct BRNode BRNode;

/**
 * @brief Struct to store data used for managing Black-Red Tree.
 */
struct BRTree {
  struct BRNode *root;  ///< Pointer to tree's root node.
  struct BRNode *guard; ///< Pointer to tree's guard.
};

/**
 * @brief Inits node with value given by argument.
 *
 * Warning! Value is not copied.
 *
 * @param memory_error : set to true if memory error has occured.
 * @param guard : pointer to BRTree guard.
 * @param value : value to initialize node with.
 * @return BRNode* : pointer to created node. (NULL if memory error has occured)
 */
static BRNode *init_node(bool *memory_error, BRNode *guard, char *value) {
  BRNode *node = wrap_malloc(sizeof(struct BRNode));
  if (node == NULL) {
    *memory_error = true;
    return NULL;
  }

  node->value = value;
  node->left_child = node->right_child = guard;
  node->is_black = false;

  return node;
}

/**
 * @brief Function compares @p s1 and @p s2 strings with lexicographic order
 * criterion.
 *
 * @param s1 : string s1 to compare.
 * @param s2 : string s2 to compare s1 with.
 * @return int : -1 if s1 < s2 | 0 if s1 == s2 | 1 if s1 > s2
 */
static int comparator(const char *s1, const char *s2) {
  while (*s1 == *s2 && *s1 != '\0' && *s2 != '\0') {
    s1 += 1;
    s2 += 1;
  }

  if (*s1 == '\0' && *s2 == '\0') {
    return 0;
  } else if (*s1 == '\0') {
    return -1;
  } else if (*s2 == '\0') {
    return 1;
  } else {
    int val1 = char_digitize(*s1);
    int val2 = char_digitize(*s2);

    // val1 == val2 can't occur because of invariant of while loop.
    if (val1 < val2) {
      return -1;
    } else {
      return 1;
    }
  }
}

/**
 * @brief Performs balancing of BRTree if node's father is left child of node's
 * grandfather.
 *
 * @param node : node to balance.
 * @param tree : pointer to the tree structure.
 * @return BRNode* : pointer to next node in balancing process.
 */
static BRNode *rbtree_repair_father_left(BRNode *node, BRTree *tree) {
  BRNode *grand_father = node->father->father;

  if (!grand_father->right_child->is_black) {
    grand_father->right_child->is_black = true;
    node->father->is_black = true;

    if (grand_father == tree->root) {
      return node;
    }
    grand_father->is_black = false;

    return grand_father;
  }

  BRNode *old_father = node->father;
  // Left Rotation
  if (node == node->father->right_child) {
    grand_father->left_child = node;
    old_father->right_child = node->left_child;
    if (node->left_child != tree->guard) {
      node->left_child->father = old_father;
    }

    node->left_child = old_father;
    node->father = grand_father;
    old_father->father = node;

    return old_father;
  } else {
    BRNode *grand_grand_father = grand_father->father;

    grand_father->left_child = old_father->right_child;
    if (old_father->right_child != tree->guard) {
      old_father->right_child->father = grand_father;
    }

    old_father->right_child = grand_father;
    grand_father->father = old_father;

    old_father->is_black = true;
    grand_father->is_black = false;

    if (grand_grand_father == tree->guard) {
      tree->root = old_father;
    } else {
      if (grand_grand_father->left_child == grand_father) {
        grand_grand_father->left_child = old_father;
      } else {
        grand_grand_father->right_child = old_father;
      }
    }
    old_father->father = grand_grand_father;

    return node;
  }
}

/**
 * @brief Performs balancing of BRTree if node's father is right child of node's
 * grandfather.
 *
 * @param node : node to balance.
 * @param tree : pointer to the tree structure.
 * @return BRNode* : pointer to next node in balancing process.
 */
static BRNode *rbtree_repair_father_right(BRNode *node, BRTree *tree) {
  BRNode *grand_father = node->father->father;

  if (!grand_father->left_child->is_black) {
    grand_father->left_child->is_black = true;
    node->father->is_black = true;

    if (grand_father == tree->root) {
      return node;
    }
    grand_father->is_black = false;

    return grand_father;
  }

  BRNode *old_father = node->father;

  if (node == node->father->left_child) {
    grand_father->right_child = node;
    old_father->left_child = node->right_child;
    if (node->right_child != tree->guard) {
      node->right_child->father = old_father;
    }

    node->right_child = old_father;
    node->father = grand_father;
    old_father->father = node;

    return old_father;
  } else {
    BRNode *grand_grand_father = grand_father->father;

    grand_father->right_child = old_father->left_child;
    if (old_father->left_child != tree->guard) {
      old_father->left_child->father = grand_father;
    }

    old_father->left_child = grand_father;
    grand_father->father = old_father;

    if (grand_grand_father == tree->guard) {
      tree->root = old_father;
    } else {
      if (grand_grand_father->left_child == grand_father) {
        grand_grand_father->left_child = old_father;
      } else {
        grand_grand_father->right_child = old_father;
      }
    }
    old_father->father = grand_grand_father;

    return node;
  }
}

/**
 * @brief Perform tree balance.
 *
 * @param tree : pointer to the tree structure.
 * @param node : pointer to node in which balancing process should start.
 */
static void rbtree_restore_order(BRTree *tree, BRNode *node) {
  while (!node->father->is_black && node == tree->root) {
    if (node->father == node->father->father->right_child) {
      node = rbtree_repair_father_right(node, tree);
    } else {
      node = rbtree_repair_father_left(node, tree);
    }
  }

  tree->root->is_black = true;
}

BRTree *init_tree(bool *memory_error) {
  BRTree *tree = wrap_malloc(sizeof(struct BRTree));
  if (tree == NULL) {
    *memory_error = true;
    return NULL;
  }

  tree->guard = wrap_calloc(1u, sizeof(struct BRNode));
  if (tree->guard == NULL) {
    *memory_error = true;
    wrap_free(tree);
    return NULL;
  }

  tree->root = tree->guard;
  tree->guard->father = tree->guard;
  return tree;
}

/**
 * @brief Function frees @p to_drop and value which it stores.
 *
 * @param to_drop : node to drop.
 */
static inline void drop_node(BRNode *to_drop) {
  wrap_free(to_drop->value);
  wrap_free(to_drop);
}

bool brtree_insert(BRTree *tree, char *to_insert) {
  bool error_occured = false;
  BRNode *new_node = init_node(&error_occured, tree->guard, to_insert);

  if (error_occured) {
    return false;
  }

  if (tree->root == tree->guard) {
    tree->root = new_node;

    new_node->father = tree->guard;
    new_node->is_black = true;

    return true;
  }

  BRNode *actual = tree->root;
  BRNode *before = tree->root;
  bool is_left = false;

  while (actual != tree->guard) {
    // TODO: Self comparing function. (DONE - TESTING)
    // int comparation = strcmp(actual->value, to_insert);
    int comparation = comparator(actual->value, to_insert);

    before = actual;
    if (comparation < 0) {
      is_left = false;
      actual = actual->right_child;
    } else if (comparation > 0) {
      is_left = true;
      actual = actual->left_child;
    } else {
      drop_node(new_node);
      return true;
    }
  }

  new_node->father = before;
  if (is_left) {
    before->left_child = new_node;
  } else {
    before->right_child = new_node;
  }

  rbtree_restore_order(tree, new_node);
  return true;
}

/**
 * @brief Frees tree with root @p node.
 *
 * Doesn't perform free on values.
 *
 * @param node : pointer to node in which deletion process should start.
 * @param guard : pointer to tree's guard.
 */
static void brtree_drop_no_values(BRNode *node, BRNode *guard) {
  if (node == guard) {
    return;
  }

  brtree_drop_no_values(node->left_child, guard);
  brtree_drop_no_values(node->right_child, guard);

  wrap_free(node);
}

/**
 * @brief Frees tree with root @p node.
 *
 * Performs free on tree's values.
 *
 * @param node : pointer to node in which deletion process should start.
 * @param guard : pointer to tree's guard.
 */
static void brtree_emergency_drop(BRNode *node, BRNode *guard) {
  if (node == guard) {
    return;
  }

  brtree_emergency_drop(node->left_child, guard);
  brtree_emergency_drop(node->right_child, guard);

  wrap_free(node->value);
  wrap_free(node);
}

/**
 * @brief Adds tree's values to @p vector array in lexicographic order.
 *
 * @param node : node to start addition from.
 * @param guard : pointer to tree's guard.
 * @param vector : dynamic array to add values to.
 * @return true : if addition process was successful.
 * @return false : if memory error has occured.
 */
static bool brtree_traverse(BRNode *node, const BRNode *guard,
                            DynamicArray *vector) {
  if (node == guard) {
    return true;
  }

  if (!brtree_traverse(node->left_child, guard, vector)) {
    return false;
  }

  bool memory_error = false;
  darray_push(vector, node->value, &memory_error);
  if (memory_error) {
    return false;
  }
  node->value = NULL;

  return brtree_traverse(node->right_child, guard, vector);
}

/**
 * @brief Frees every value (strings) from given dynamic array.
 *
 * @param array : dynamic array to clean.
 */
static void darray_string_drop(DynamicArray *array) {
  size_t array_size = darray_size(array);
  char **array_chars = (char **)darray_convert(array);

  for (size_t ind = 0; ind < array_size; ind++) {
    wrap_free(array_chars[ind]);
  }

  wrap_free(array_chars);
}

DynamicArray *brtree_conversion(BRTree *tree, bool *memory_error) {
  bool mem_error_result = false;

  DynamicArray *vector = init_darray(&mem_error_result);
  if (mem_error_result) {
    *memory_error = true;
    return NULL;
  }

  if (!brtree_traverse(tree->root, tree->guard, vector)) {
    darray_string_drop(vector);

    brtree_emergency_drop(tree->root, tree->guard);
    wrap_free(tree->guard);
    wrap_free(tree);

    *memory_error = true;
    return NULL;
  } else {
    brtree_drop_no_values(tree->root, tree->guard);
  }

  wrap_free(tree->guard);
  wrap_free(tree);

  return vector;
}

void brtree_drop(BRTree *tree) {
  brtree_emergency_drop(tree->root, tree->guard);

  wrap_free(tree->guard);
}