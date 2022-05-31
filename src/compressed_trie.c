/**
 * @file compressed_trie.c
 * @author Przemysław Fuchs (pf438429@students.mimuw.edu.pl)
 * @brief Module implements compressed trie data structure.
 * @date 2022-05-07
 */
#include "compressed_trie.h"
#include "dynamic_array.h"
#include "memory.h"
#include "string_lib.h"
#include <assert.h>
#include <string.h>

/**
 * @brief Defines how many children node can have.
 */
#define MAX_NUMBER_OF_CHILDREN 12

/**
 * @brief Defines how long buffer for trie traversing should be
 * at tree init.
 */
#define INIT_BUFFER_SIZE 10

struct TrieChild;
/**
 * @brief Typedef shortens TrieChild name to make code more readable.
 */
typedef struct TrieChild TrieChild;

/**
 * @brief Represents children reference of given node.
 */
struct TrieChild {
  struct TrieNode *child; ///< Pointer to children node.
  char *edge_etiquette;   ///< Etiquetee of parent-children edge.
};

/**
 * @brief Represents node of compressed trie.
 */
struct TrieNode {
  struct TrieNode *father;                    ///< Pointer to father of node.
  TrieChild children[MAX_NUMBER_OF_CHILDREN]; ///< Array which stores
                                              ///< references to children.

  void *value; ///< Value of given node (with key which is determined by path
               ///< from root).
};

/**
 * @brief Structure to be used by the Trie library user.
 *  Provides a trie for strings which consists of digits.
 */
struct Trie {
  struct TrieNode *root; ///< Pointer to the root of the trie.
  void (*value_free_function)(
      void *value, const char *key,
      void *configuration);  ///< Function to be called at values at
                             ///< the moment of Trie deletion.
  size_t longest_key;        ///< Length of the longest key in trie.
  char *longest_key_buffer;  ///< Buffer to store strings of size longest_key+1.
  void *free_wrapper_config; ///< Pointer which is passed to value_free_function
};

/**
 * @brief Inits empry trienode with all values be either NULL or zero.
 *
 * @param[out] memory_error_occured : setted to true if allocation error occurs.
 * @return TrieNode* : created node. (NULL if error occured).
 */
static TrieNode *init_empty_trienode(bool *memory_error_occured) {
  TrieNode *node = wrap_malloc(sizeof(struct TrieNode));

  if (node == NULL) {
    *memory_error_occured = true;
    return NULL;
  }

  node->father = NULL;

  for (size_t index = 0; index < MAX_NUMBER_OF_CHILDREN; index++) {
    node->children[index].child = NULL;
    node->children[index].edge_etiquette = NULL;
  }

  node->value = NULL;

  return node;
}

/**
 * @brief Drops one trienode without modifying anything else.
 *  Also perform deletion operation on node's value.
 *
 * @param[in] node : TrieNode to drop.
 * @param[in] tree : @p node 's Trie.
 */
static inline void trie_drop_one_node(TrieNode *node, const Trie *tree) {
  if (node->value != NULL) {
    tree->value_free_function(node->value, NULL, tree->free_wrapper_config);
  }

  wrap_free(node);
}

/**
 * @brief Function to fix conflict in compressed trie (if two nodes shares some
 *  common prefix but they need seperate branches in trie).
 *
 * @param[in] tree: pointer to the Trie, at which operation is performed.
 * @param[in, out] node : pointer to node which has conflicting etiquette.
 * @param[in] key : key of node to perform addition.
 * @param char_no : index of character in @p key where conflict begins (start of
 * labeling).
 * @param prefix_size : common prefix size of @p key and @p node coflicting edge
 * etiquette.
 * @param[out] new_node : place to write pointer to created node to.
 * @return true : if operation succedes.
 * @return false : if operation failes (nothing changes).
 */
static bool trie_conflict(Trie *tree, TrieNode *node, const char *key,
                          size_t char_no, size_t prefix_size,
                          TrieNode **new_node) {
  bool error_occured = false;

  size_t next_int = char_digitize(key[char_no]);
  TrieNode *child = init_empty_trienode(&error_occured);

  if (error_occured) {
    return false;
  }

  char *old_str = node->children[next_int].edge_etiquette;
  TrieNode *old_child = node->children[next_int].child;

  size_t old_ind = char_digitize(old_str[prefix_size]);
  if (char_no + prefix_size == strlen(key)) {
    child->children[old_ind].edge_etiquette =
        string_clone_from_index(old_str, prefix_size);
    if (child->children[old_ind].edge_etiquette == NULL) {
      trie_drop_one_node(child, tree);
      return false;
    }

    string_cut_at_char(&node->children[next_int].edge_etiquette, prefix_size);
    node->children[next_int].child = child;
    child->children[old_ind].child = old_child;
    old_child->father = child;
    child->father = node;
    *new_node = child;

    return true;
  }

  size_t key_ind = char_digitize(key[char_no + prefix_size]);

  child->children[key_ind].edge_etiquette =
      string_clone_from_index(key, char_no + prefix_size);
  if (child->children[key_ind].edge_etiquette == NULL) {
    trie_drop_one_node(child, tree);
    return false;
  }

  child->children[key_ind].child = init_empty_trienode(&error_occured);
  if (error_occured) {
    wrap_free(child->children[key_ind].edge_etiquette);
    trie_drop_one_node(child, tree);
    return false;
  }

  TrieNode *new_child = child->children[key_ind].child;
  new_child->father = child;

  child->children[old_ind].edge_etiquette =
      string_clone_from_index(old_str, prefix_size);
  if (child->children[old_ind].edge_etiquette == NULL) {
    wrap_free(child->children[old_ind].edge_etiquette);
    wrap_free(child->children[key_ind].edge_etiquette);
    trie_drop_one_node(child, tree);
    return false;
  }

  child->children[old_ind].child = old_child;
  old_child->father = child;
  string_cut_at_char(&node->children[next_int].edge_etiquette, prefix_size);

  node->children[next_int].child = child;
  child->father = node;

  *new_node = new_child;
  return true;
}

/**
 * @brief Perform search of node of given key corresponding to @p key.
 *
 * @param[in] beggining : pointer to node from which search must begin.
 * @param[out] result : place to save result of the succesful search.
 * @param[in] key : key of node for which function perform searching.
 * @param[out] father_result_index : index in father's child array where
 *  found node's reference is stored.
 * @return true : if node was found.
 * @return false : if node was not found.
 */
static bool search_node(TrieNode *beggining, TrieNode **result, const char *key,
                        size_t *father_result_index) {
  size_t actual_char = 0;
  size_t key_length = strlen(key);

  while (beggining != NULL) {
    if (actual_char == key_length) {
      *result = beggining;
      return true;
    }

    size_t digit = char_digitize(key[actual_char]);
    size_t pref_len = 0;
    const char *etiq = beggining->children[digit].edge_etiquette;

    if (etiq == NULL) {
      return false;
    } else if (string_check_prefixes(key, actual_char, etiq, &pref_len)) {
      beggining = beggining->children[digit].child;
      actual_char += pref_len;
      *father_result_index = digit;
    } else {
      return false;
    }
  }

  return false;
}

/**
 * @brief Function founds value of longest prefix of @p key stored in key.
 *
 * Result of search is value of node K which satisfies following conditions:
 * 1) forall (Node M in tree) Key(M) is prefix of @p key -> Key(M) is prefix of
 * Key(K).
 *
 * @param[in] beggining : pointer to node from which search begins.
 * @param[in] key : string of digits for search for.
 * @param[out] longest_pref_size : saves length of longest matched prefix.
 * @return const char* : value of node with key which is longest prefix.
 */
static void *search_longest_prefix(TrieNode *beggining, const char *key,
                                   size_t *longest_pref_size) {
  size_t actual_char = 0;
  size_t key_length = strlen(key);
  void *result = NULL;

  while (beggining != NULL) {
    if (actual_char == key_length) {
      return result;
    }

    size_t digit = char_digitize(key[actual_char]);
    size_t pref_len = 0;
    const char *etiq = beggining->children[digit].edge_etiquette;

    if (etiq == NULL) {
      return result;
    } else if (string_check_prefixes(key, actual_char, etiq, &pref_len)) {
      beggining = beggining->children[digit].child;
      actual_char += pref_len;

      if (beggining->value != NULL) {
        *longest_pref_size = actual_char;
        result = beggining->value;
      }
    } else {
      return result;
    }
  }

  return result;
}

/**
 * @brief Function checks is node of given @p key exists in Trie. If not it
 * inserts new node and returns pointer.
 *
 * @param[in, out] tree : pointer to the processed Trie.
 * @param[in] key : string of digits to match node's key for.
 * @param[out] check_result : place to save search / insertion result.
 * @return true : if operation succeded.
 * @return false : if operation failed.
 */
static bool trie_check_add_node(Trie *tree, const char *key,
                                TrieNode **check_result) {
  TrieNode *root = tree->root;
  bool error_occured = false;
  TrieNode *node = root;
  assert(node != NULL);
  size_t char_no = 0;
  const size_t key_len = strlen(key);

  while (true) {
    if (char_no == key_len) {
      *check_result = node;
      return true;
    }

    size_t next_digit = char_digitize(key[char_no]);

    if (node->children[next_digit].child == NULL) {
      TrieNode *child = node->children[next_digit].child =
          init_empty_trienode(&error_occured);
      if (error_occured) {
        return false;
      }

      child->father = node;

      node->children[next_digit].edge_etiquette =
          string_clone_from_index(key, char_no);
      if (node->children[next_digit].edge_etiquette == NULL) {
        trie_drop_one_node(child, tree);
        node->children[next_digit].child = NULL;
        return false;
      }

      *check_result = node->children[next_digit].child;
      return true;
    }

    char *edge_label = node->children[next_digit].edge_etiquette;

    size_t common_prefix_size = 0;
    if (string_check_prefixes(key, char_no, edge_label, &common_prefix_size)) {
      node = node->children[next_digit].child;
      char_no += common_prefix_size;
    } else {
      return trie_conflict(tree, node, key, char_no, common_prefix_size,
                           check_result);
    }
  }
}

/**
 * @brief Recursively drop compressed trie.
 *
 * @param[in, out] tree : pointer to the Trie to drop.
 * @param[in] node : pointer to the node currently dropped.
 * @param buf_first_free_index : index of first free plance in @p tree 's
 * key_buffer.
 */
static void trienode_drop(Trie *tree, TrieNode *node,
                          size_t buf_first_free_index) {
  if (node == NULL) {
    return;
  }

  if (node->value != NULL) {
    if (tree->longest_key_buffer != NULL) {
      tree->longest_key_buffer[buf_first_free_index] = '\0';
    }

    tree->value_free_function(node->value, tree->longest_key_buffer,
                              tree->free_wrapper_config);
  }

  for (size_t index = 0; index < MAX_NUMBER_OF_CHILDREN; index++) {
    if (node->children[index].child != NULL) {
      size_t etiq_size = strlen(node->children[index].edge_etiquette);

      if (tree->longest_key_buffer != NULL) {
        for (size_t ind = buf_first_free_index;
             ind < buf_first_free_index + etiq_size; ind++) {
          tree->longest_key_buffer[ind] =
              node->children[index].edge_etiquette[ind - buf_first_free_index];
        }
      }

      trienode_drop(tree, node->children[index].child,
                    buf_first_free_index + etiq_size);
      wrap_free(node->children[index].edge_etiquette);

      node->children[index].edge_etiquette = NULL;
      node->children[index].child = NULL;
    }
  }

  wrap_free(node);
}

/**
 * @brief Performs tree balancing after node deletion.
 *
 * Removes nodes with only one child and compressed them to one node.
 * If memory error occured (string concating requires mem allocation) then
 * balancing is terminated, but Trie structure remains consistent and working.
 *
 * @param[in, out] node : pointer to node, from which balancing process should
 * start.
 */
static void trie_balance(TrieNode *node) {
  if (node == NULL || node->father == NULL) {
    return;
  }

  size_t node_children = 0;
  size_t child_index = 11;

  for (size_t digit = 0; digit < MAX_NUMBER_OF_CHILDREN; digit++) {
    if (node->children[digit].child != NULL) {
      node_children++;
      child_index = digit;
    }
  }

  while (true) {
    if (node->father == NULL) {
      return;
    }
    TrieNode *father = node->father;
    size_t old_child_count = node_children;
    size_t old_child_index = child_index;
    size_t my_index_at_father = 11;
    node_children = 0;

    for (size_t digit = 0; digit < MAX_NUMBER_OF_CHILDREN; digit++) {
      if (father->children[digit].child != NULL) {
        node_children++;
        if (father->children[digit].child == node) {
          my_index_at_father = digit;
        } else {
          child_index = digit;
        }
      }
    }

    if (old_child_count == 0 && node->value != NULL) {
      return;
    } else if (old_child_count == 0 && node->value == NULL) {
      wrap_free(node);

      father->children[my_index_at_father].child = NULL;
      wrap_free(father->children[my_index_at_father].edge_etiquette);

      node_children -= 1;
      node = father;
    }
    if (old_child_count == 1 && node->value == NULL) {
      if (string_concat(&father->children[my_index_at_father].edge_etiquette,
                        node->children[old_child_index].edge_etiquette)) {
        father->children[my_index_at_father].child =
            node->children[old_child_index].child;
        father->children[my_index_at_father].child->father = father;
        wrap_free(node->children[old_child_index].edge_etiquette);
        wrap_free(node);

        node = father;
      } else {
        // No further simplification of the tree can be done without
        // allocating a little memory for concatenation.
        break;
      }

    } else {
      break;
    }
  }
}

/**
 * @brief Performs cleaning and deletion of single node @p node.
 *
 * Does something if one of two situations has occured:
 * 1) @p node has 0 children, then it drops @p node and starts balacing tree
 * from
 * @p node father. 2) @p node has 1 child, then it connects this child to @p
 * node father and drops @p node.
 *
 * @param[in] node : Node to delete from the tree.
 * @param index : index of @p in father's children array.
 */
static void trie_node_purge(TrieNode *node, size_t index) {
  TrieNode *father = node->father;
  size_t node_children = 0;
  size_t child_index = 11;

  for (size_t digit = 0; digit < MAX_NUMBER_OF_CHILDREN; digit++) {
    if (node->children[digit].child != NULL) {
      node_children++;
      child_index = digit;
    }
  }

  if (node_children == 0) {
    wrap_free(node);

    wrap_free(father->children[index].edge_etiquette);
    father->children[index].edge_etiquette = NULL;
    father->children[index].child = NULL;
    trie_balance(father);
  } else if (node_children == 1) {
    string_concat(&father->children[index].edge_etiquette,
                  node->children[child_index].edge_etiquette);

    wrap_free(node->children[child_index].edge_etiquette);

    node->children[child_index].child->father = father;
    father->children[index].child = node->children[child_index].child;

    wrap_free(node);
  }
}

// ============================================================
// Public interface functions.

Trie *init_trie(bool *memory_error,
                void (*value_free_function)(void *value, const char *key,
                                            void *configuration),
                void *free_wrapper_configuration) {
  bool error_occured = false;

  TrieNode *root = init_empty_trienode(&error_occured);
  if (error_occured) {
    *memory_error = true;
    return NULL;
  }

  Trie *tree = wrap_malloc(sizeof(struct Trie));
  if (tree == NULL) {
    wrap_free(root);
    *memory_error = true;
    return NULL;
  }

  tree->longest_key_buffer = wrap_malloc(sizeof(char) * (INIT_BUFFER_SIZE + 1));
  if (tree->longest_key_buffer == NULL) {
    wrap_free(tree);
    wrap_free(root);
    *memory_error = true;
    return NULL;
  }

  tree->free_wrapper_config = free_wrapper_configuration;
  tree->longest_key = INIT_BUFFER_SIZE;
  tree->root = root;
  tree->value_free_function = value_free_function;

  return tree;
}

void trie_remove(Trie *tree, const char *key) {
  TrieNode *node = NULL;
  size_t index_k = 0;

  if (search_node(tree->root, &node, key, &index_k)) {
    tree->value_free_function(node->value, key, tree->free_wrapper_config);
    node->value = NULL;
    trie_node_purge(node, index_k);
  }
}

void trie_remove_from_ptr(Trie *tree, TrieNode *node, const char *key) {
  TrieNode *father = node->father;

  size_t child_index = 0;
  bool found_child = false;
  for (size_t index = 0; index < MAX_NUMBER_OF_CHILDREN && !found_child;
       index++) {
    if (father->children[index].child == node) {
      child_index = index;
      found_child = true;
    }
  }

  assert(found_child);

  tree->value_free_function(node->value, key, tree->free_wrapper_config);
  node->value = NULL;

  trie_node_purge(node, child_index);
}

TrieNode *trie_insert(Trie *tree, const char *key, void *value) {
  if (value == NULL) {
    return NULL;
  }

  size_t key_length = strlen(key);
  if (tree->longest_key < key_length) {
    char *new_buffer =
        wrap_realloc(tree->longest_key_buffer, sizeof(char) * (key_length + 1));

    if (new_buffer == NULL) {
      return NULL;
    } else {
      tree->longest_key_buffer = new_buffer;
      tree->longest_key = key_length;
    }
  }

  TrieNode *node = NULL;
  if (trie_check_add_node(tree, key, &node)) {
    void *prev_value = node->value;

    node->value = value;
    const char *provided_key = key;
    if (node->value == NULL) {
      provided_key = NULL;
    }

    tree->value_free_function(prev_value, provided_key,
                              tree->free_wrapper_config);
  } else {
    return NULL;
  }

  return node;
}

void *trie_match_longest_prefix(const Trie *tree, const char *key,
                                size_t *matched_length) {
  return search_longest_prefix(tree->root, key, matched_length);
  // TODO: Na wyższym poziomie trzeba będzie obsłużyć to co niżej. (Wygląda
  // jakby było obsłużone.)
  /**
  size_t val_len = strlen(value);

  size_t res_len = val_len + (strlen(key) - pref_len);

  char *res = wrap_malloc(sizeof(char) * (res_len + 1));
  if (res == NULL) {
    *memory_error = true;
    return NULL;
  }

  memcpy(res, value, val_len * sizeof(char));
  strcpy(res + val_len, key + pref_len);
  return res; */
}

void trie_remove_subtree(Trie *tree, const char *prefix) {
  size_t input_len = strlen(prefix);
  size_t actual_char = 0;
  TrieNode *actual = tree->root;
  size_t father_child_index = 11;
  size_t buffer_free_index = 0;

  while (input_len > actual_char) {
    size_t pref_len;

    size_t node_ind = char_digitize(prefix[actual_char]);

    char *to_check = actual->children[node_ind].edge_etiquette;
    if (to_check == NULL) {
      return;
    }

    if (!string_check_prefixes(prefix, actual_char, to_check, &pref_len) &&
        actual_char + pref_len != input_len) {
      return;
    }

    char *etiq = actual->children[node_ind].edge_etiquette;
    while (*etiq != '\0') {
      tree->longest_key_buffer[buffer_free_index] = *etiq;
      buffer_free_index++;

      etiq += 1;
    }

    actual = actual->children[node_ind].child;

    father_child_index = node_ind;
    actual_char += pref_len;
  }

  TrieNode *actual_father = actual->father;

  trienode_drop(tree, actual, buffer_free_index);

  if (actual_father != NULL) {
    actual_father->children[father_child_index].child = NULL;

    wrap_free(actual_father->children[father_child_index].edge_etiquette);
    actual_father->children[father_child_index].edge_etiquette = NULL;
  }

  trie_balance(actual_father);
}

void trie_drop(Trie *tree) {
  if (tree == NULL) {
    return;
  }

  trienode_drop(tree, tree->root, 0);

  wrap_free(tree->longest_key_buffer);
  wrap_free(tree);
}

void *trie_locate_node(Trie *tree, const char *key, void *value,
                       TrieNode **located_node) {
  TrieNode *search_result;

  if (trie_check_add_node(tree, key, &search_result)) {
    if (search_result->value == NULL) {
      search_result->value = value;
      *located_node = search_result;
      return value;
    } else {
      *located_node = search_result;
      return search_result->value;
    }
  } else {
    return NULL;
  }
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

#include "double_linked_list.h"
// TODO: MEMORY ERROR MANAGEMENT. (DONE - TESTING)
DynamicArray *trie_traverse_down(const Trie *tree, const char *key) {
  if (key == NULL || tree == NULL) {
    return NULL;
  }

  bool memory_error = false;

  DynamicArray *array = init_darray(&memory_error);
  if (memory_error) {
    return NULL;
  }

  size_t actual_char = 0;
  size_t key_len = strlen(key);
  TrieNode *node = tree->root;

  while (node != NULL) {
    if (node->value != NULL) {
      ListIterator *iterator = list_iterator(node->value, &memory_error);
      if (memory_error) {
        darray_string_drop(array);
        return NULL;
      }

      while (listiterator_has_next(iterator)) {
        const char *value = listiterator_next(iterator);

        size_t val_len = strlen(value);
        char *element =
            wrap_malloc(sizeof(char) * (key_len - actual_char + val_len + 1));
        if (element == NULL) {
          listiterator_drop(iterator);
          darray_string_drop(array);
          return NULL;
        }

        memcpy(element, value, sizeof(char) * val_len);
        strcpy(element + val_len, key + actual_char);

        darray_push(array, element, &memory_error);
      }

      listiterator_drop(iterator);
    }

    if (actual_char == key_len) {
      return array;
    }

    size_t digit = char_digitize(key[actual_char]);
    size_t pref_len = 0;
    const char *etiq = node->children[digit].edge_etiquette;

    if (etiq == NULL) {
      return array;
    } else if (string_check_prefixes(key, actual_char, etiq, &pref_len)) {
      node = node->children[digit].child;
      actual_char += pref_len;
    } else {
      return array;
    }
  }

  return array;
}

void *trienode_get_value(TrieNode *node) { return node->value; }