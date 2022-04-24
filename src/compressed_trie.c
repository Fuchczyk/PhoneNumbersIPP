#include "compressed_trie.h"
#include "memory.h" 
#include "string_lib.h"
#include <string.h>
#include <assert.h>
#include <stdio.h>

#define MAX_NUMBER_OF_CHILDREN 10

struct TrieChild;
typedef struct TrieChild TrieChild;

struct TrieNode;

/**
 * @brief Represents children reference of given node.
 */
struct TrieChild {
    struct TrieNode *child; ///< Pointer to children node.
    char *edge_etiquette; //;< Etiquetee of parent-children edge.
};

/**
 * @brief Represents node of compressed trie.
 */
struct TrieNode {
    struct TrieNode *father; ///< Pointer to father of node.
    TrieChild children[MAX_NUMBER_OF_CHILDREN]; ///< Array which stores referencess to children.

    char *value; ///< Value connected to node.
};
typedef struct TrieNode TrieNode;

struct Trie {
    struct TrieNode *root;
};

static inline size_t char_digitize(char c) {
    return (size_t) (c - '0');
}

static TrieNode *init_empty_trienode(bool *memory_error_occured) {
    TrieNode *node = wrap_malloc(sizeof(struct TrieNode));

    if (node == NULL) {
        *memory_error_occured = true;
        return NULL;
    }

    node->father = NULL;

    for (size_t index = 0 ; index < MAX_NUMBER_OF_CHILDREN ; index++) {
        node->children[index].child = NULL;
        node->children[index].edge_etiquette = NULL;
    }

    node->value = NULL;

    return node;
}

static inline void trie_drop_one_node(TrieNode *node) {
    wrap_free(node->father);
    wrap_free(node->value);

    wrap_free(node);
}

static bool trie_conflict(
    TrieNode *node,
    const char *key,
    size_t char_no,
    size_t prefix_size,
    TrieNode **new_node
) {
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
            trie_drop_one_node(child);
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

    assert(key_ind != old_ind);
    
    child->children[key_ind].edge_etiquette =
        string_clone_from_index(key, char_no + prefix_size);
    if (child->children[key_ind].edge_etiquette == NULL) {
        trie_drop_one_node(child);
        return false;
    }

    child->children[key_ind].child = init_empty_trienode(&error_occured);
    if (error_occured) {
        wrap_free(child->children[key_ind].edge_etiquette);
        trie_drop_one_node(child);
        return false;
    }

    TrieNode *new_child = child->children[key_ind].child;
    new_child->father = child;
   
    child->children[old_ind].edge_etiquette =
        string_clone_from_index(old_str, prefix_size);
    if (child->children[old_ind].edge_etiquette == NULL) {
        wrap_free(child->children[old_ind].edge_etiquette);
        wrap_free(child->children[key_ind].edge_etiquette);
        trie_drop_one_node(child);
        return false;
    }

    child->children[old_ind].child = old_child;
    old_child->father = child;
    string_cut_at_char(&node->children[next_int].edge_etiquette, prefix_size);

    node->children[next_int].child = child;
    child->father = node;

    *new_node = new_child;
    return true; //TODO: MEMORY OUT OF DETECTION
}

static bool search_node(
    TrieNode *beggining,
    TrieNode **result,
    const char *key,
    size_t *father_result_index
) {
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

static const char *search_longest_prefix(
    TrieNode *beggining,
    const char *key,
    size_t *longest_pref_size
) {
    size_t actual_char = 0;
    size_t key_length = strlen(key);
    const char *result = NULL;
    TrieNode *a = NULL;

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
            a = beggining;
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

static bool trie_check_add_node(TrieNode *root, const char *key, TrieNode **check_result) {
    bool error_occured = false;
    TrieNode *node = root;
    size_t char_no = 0;
    const size_t key_len = strlen(key);

    while (true) {
        if (char_no == key_len) {
            *check_result = node;
            return true;
        }

        size_t next_digit = char_digitize(key[char_no]);

        if (node->children[next_digit].child == NULL) {
            TrieNode *child = node->children[next_digit].child = init_empty_trienode(&error_occured);
            if (error_occured) {
                return false;
            }

            child->father = node;

            node->children[next_digit].edge_etiquette =
                string_clone_from_index(key, char_no);
            if (node->children[next_digit].edge_etiquette == NULL) {
                trie_drop_one_node(child);
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
            return trie_conflict(node, key, char_no, common_prefix_size, check_result);
        }
    }
}

Trie *init_trie(bool *memory_error) {
    bool error_occured = false;

    TrieNode *root = init_empty_trienode(&error_occured);
    if (error_occured) {
        *memory_error = true;
        return NULL;
    }

    Trie *tree = wrap_malloc(sizeof(struct Trie));
    if (tree == NULL) {
        trie_drop_one_node(root);
        *memory_error = true;
        return NULL;
    }

    tree->root = root;

    return tree;
}

static void trienode_drop(TrieNode *node) {
    if (node == NULL) {
        return;
    }

    for (size_t index = 0 ; index < MAX_NUMBER_OF_CHILDREN ; index++) {
        trienode_drop(node->children[index].child);
        wrap_free(node->children[index].edge_etiquette);
    }

    wrap_free(node->value);

    wrap_free(node);
}

static void trie_balance(TrieNode *node) {
    if (node == NULL || node->father == NULL) {
        return;
    }

    size_t node_children = 0;
    size_t child_index = 11;

    for (size_t digit = 0 ; digit <= 9 ; digit++) {
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

        for (size_t digit = 0 ; digit <= 9 ; digit++) {
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
            assert(node_children != 0);

            father->children[my_index_at_father].child = NULL;
            wrap_free(father->children[my_index_at_father].edge_etiquette);

            node_children -= 1;
            node = father;
        }
        if (old_child_count == 1 && node->value == NULL) {
            
            assert(my_index_at_father != 11);

            // TODO: ODWRACALNOŚĆ
            string_concat(&father->children[my_index_at_father].edge_etiquette,
                node->children[old_child_index].edge_etiquette);
            father->children[my_index_at_father].child = 
                node->children[old_child_index].child;
            father->children[my_index_at_father].child->father = father;
            wrap_free(node->children[old_child_index].edge_etiquette);
            wrap_free(node);

            node = father;
            assert(node_children != 0);
            //node_children -= 1;
        } else {
            break;
        }
    }
}

static void trie_node_purge(TrieNode *node, size_t index) {
    TrieNode *father = node->father;
    size_t node_children = 0;
    size_t child_index = 11;

    for (size_t digit = 0 ; digit <= 9 ; digit++) {
        if (node->children[digit].child != NULL) {
            node_children++;
            child_index = digit;
        }
    }
    assert(child_index != 11 || node_children == 0);

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
        father->children[index].child = 
            node->children[child_index].child;
        
        wrap_free(node);
    }
}

//TODO: INSERT MOŻE OPEROWAĆ NA CHAR POINTERACH
bool trie_insert(Trie *tree, const char *key, const char *value) {
    TrieNode *node = NULL;
    if (trie_check_add_node(tree->root, key, &node)) {
        char *prev_value = node->value;

        node->value = string_clone(value);
        if (node->value == NULL) {
            if (prev_value != NULL) {
                node->value = prev_value;
            } else { //FIXME: Naprawić błąd logiczny : przecież (tree key) nie będzie identyfikowalny
                trie_remove(tree, key);
            }
            return false;
        } else {
            wrap_free(prev_value);
        }

    } else {
        return false;
    }

    return true;
}


void trie_remove(Trie *tree, const char *key) {
    TrieNode *node = NULL;
    size_t index_k = 0;

    if (search_node(tree->root, &node, key, &index_k)) {
        const char *value = node->value;

        wrap_free(node->value);
        node->value = NULL;
        trie_node_purge(node, index_k);
    }
}

static void trienode_debug_print(TrieNode *node);

void trie_remove_subtree(Trie *tree, const char *prefix) {
    size_t input_len = strlen(prefix);
    size_t actual_char = 0;
    TrieNode *actual = tree->root;
    size_t father_child_index = 11;
    bool found_tree = false;

    while (input_len > actual_char) {
        size_t pref_len;

        size_t node_ind = char_digitize(prefix[actual_char]);
        
        char *to_check = actual->children[node_ind].edge_etiquette;
        if (to_check == NULL) {
            printf("TRIE REMOVE FOUND NOTHING 1.\n");
            printf("DO USUNIECIA BYŁO (1): %s\n", prefix);
            return;
        }

        if (string_check_prefixes(prefix, actual_char, to_check, &pref_len)) {
            //
        } else if (actual_char + pref_len != input_len) {
            printf("TRIE REMOVE FOUND NOTHING 2.\n");
            return;
        }
        actual = actual->children[node_ind].child;

        father_child_index = node_ind;
        actual_char += pref_len;
    }

    /* printf("TRIE REMOVE FOUND SOMETHING OF VALUE %s AT INDEX %zu\n", actual->value, father_child_index);
    printf("FOUND POINTER %p\n", actual);
    trienode_debug_print(actual); */

    TrieNode *actual_father = actual->father;

    trienode_drop(actual);

    if (actual_father != NULL) { // == NULL if node is root of the tree
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

    trienode_drop(tree->root);
    wrap_free(tree);
}

static void trienode_debug_print(TrieNode *node) {
    if (node == NULL) {
        return;
    }

    printf("WYPISUJĘ SIEBIE\n");
    if (node->value == NULL) {
        printf("Moja etykieta value to NULL.\n");
    } else {
        printf("Moja etykieta value to %s\n", node->value);
    }

    printf("Wypisuję dzieci.\n");
    for (size_t index = 0 ; index < MAX_NUMBER_OF_CHILDREN ; index++) {
        if (node->children[index].edge_etiquette == NULL) {
            printf("ETYKIETA DZIECKA %zu to NULL.\n", index);
        } else {
            printf("ETYKIETA DZIECKA %zu to %s.\n", index, node->children[index].edge_etiquette);
        }
        trienode_debug_print(node->children[index].child);
    }
}

void trie_debug_print(Trie *tree) {
    trienode_debug_print(tree->root);
}

char *trie_match_longest_prefix(
    const Trie *tree,
    const char *key,
    bool *memory_error
) {
    size_t pref_len = 0;
    const char *value = search_longest_prefix(tree->root, key, &pref_len);
    if (value == NULL) {
        return NULL;
    }
    
    size_t val_len = strlen(value);

    size_t res_len = val_len + (strlen(key) - pref_len);

    char *res = wrap_malloc(sizeof(char) * (res_len + 1));
    if (res == NULL) {
        *memory_error = true;
        return NULL;
    }

    strncpy(res, value, val_len);
    strcpy(res + val_len, key + pref_len);
    return res;
}