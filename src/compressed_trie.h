/**
 * @file compressed_trie.h
 * @author Przemysław Fuchs (pf438429@students.mimuw.edu.pl)
 * @brief Inteface of module implementing Trie data structure.
 *
 * Key and values should be in form of digit strings.
 * Tries store key <-> value pairs and allows to perform match longest prefix
 * operation and key <-> value pair deletion and insertion.
 *
 * @date 2022-05-07
 */
#ifndef __COMPRESSED_TRIE_H__
#define __COMPRESSED_TRIE_H__
#include "dynamic_array.h"
#include <stdbool.h>
#include <stddef.h>

/**
 * @brief Struct which user uses to specify, which node should be deleted from
 * the tree.
 */
struct TrieNode;

/**
 * @brief Typedef to keep code clean and more readable.
 */
typedef struct TrieNode TrieNode;

/**
 * @brief Trie to store pairs: Key<String> <-> Key<Value>,
 *  only one value can be connected to given key. Keys needs to be numbers -
 * strings which consists of digits.
 */
struct Trie;
/**
 * @brief Typedef shortens Trie name to keep code more readable.
 */
typedef struct Trie Trie;

/**
 * @brief Function inits compressed trie data structre.
 *
 * @param[out] memory_error : indicates if the was a memory error.
 * @param value_free_function : pointer to function which is used to free node's
 * value. [value - pointer to node's value to free, key - const pointer to
 * corresponded key, configuration - pointer which is passed to function (may be
 * used to provide some more configuration to user's function)]
 * @param[in] free_wrapper_configuration : pointer to configuration which is
 * passed to @p value_free_function.
 * @return Trie* : created data structure.
 */
Trie *init_trie(bool *memory_error,
                void (*value_free_function)(void *value, const char *key,
                                            void *configuration),
                void *free_wrapper_configuration);

/**
 * @brief Function inserts key - value pair to the trie structure.
 *  If trie has already a value conntected to given key, it becomes overwritten.
 *
 * Warning! Function doesn't check if given string are valid.
 *
 * @param[in, out] tree : pointer to trie in which we want to perform insertion.
 * @param[in] key : String key (digit char string).
 * @param[in] value : pointer (value to insert).
 * @return true : if insertion was successful.
 * @return false : if error has occured (structure state has not been modified).
 */
TrieNode *trie_insert(Trie *tree, const char *key, void *value);

/**
 * @brief Returns value of the longest prefix of @p key that occurs in trie.
 *
 * String key must consist of digits, eg. "8392#*"
 * If there is not such prefix, it returns NULL.
 *
 * @param[in] tree : trie to perform search in.
 * @param[in] key : key to match.
 * @param[out] matched_length : place to save length of matched common prefix.
 * @return void* : found value.
 */
void *trie_match_longest_prefix(const Trie *tree, const char *key,
                                size_t *matched_length);

/**
 * @brief Removes all (key, value) pairs such that key has prefix which equals
 * @p prefix.
 *
 * @param[in, out] tree : trie to remove data from.
 * @param[in] prefix : prefix of key to delete.
 */
void trie_remove_subtree(Trie *tree, const char *prefix);

/**
 * @brief Deletes Trie data structure.
 *
 * @param[in] tree : Trie to delete.
 */
void trie_drop(Trie *tree);

/**
 * @brief Searches for node of given key and add if it wasn't in the tree.
 *
 * If node has not existed before, it inserts a node with value @p value and
 * returns @p value. If node has existed, it returns pointer to node value (it
 * doesn't do any operation on argument @p value).
 *
 * If memory error has occured, it returns NULL.
 *
 * @param[in, out] tree : Trie to perform operation at.
 * @param[in] key : key of node to locate.
 * @param[in] value : value to insert if node of given @p key doesn't exist.
 * @param[out] located_node : place to save pointer to located node.
 * @return void* : pointer to node value or NULL (if error occured).
 */
void *trie_locate_node(Trie *tree, const char *key, void *value,
                       TrieNode **located_node);

/**
 * @brief Removes node of specified key by @p key from the tree.
 *
 * @param[in, out] tree : Trie to delete node from.
 * @param[in] key : key of node to delete.
 */
void trie_remove(Trie *tree, const char *key);

/**
 * @brief Unsafe function for remove some node from trie by pointer to that
 * node.
 *
 * @param[in, out] tree : Trie to remove @p node from.
 * @param[in] node : pointer to the node to remove.
 * @param[in] key : key which was used to insertion of the node.
 */
void trie_remove_from_ptr(Trie *tree, TrieNode *node, const char *key);

/**
 * @brief Function collects values from all prefixes (keys) of @p key.
 *
 * @param[in] tree : Trie to collect values from.
 * @param[in] key : key to collect all prefixes of.
 * @return DynamicArray* : array of collected values (Trie doesn't transfer
 * pointer ownership ; return NULL if memory error has occured)
 */
DynamicArray *trie_traverse_down(const Trie *tree, const char *key);

/**
 * @brief Function to collect value from Trie node given by the pointer.
 *
 * @param[in] node : pointer of node to collect value from.
 * @return void* : collected value (pointer ownership is not transfered).
 */
void *trienode_get_value(TrieNode *node);

#endif /* __COMPRESSED_TRIE_H__ */