/**
 * @file compressed_trie.h
 * @author Przemysław Fuchs (fuchs.przemyslaw@gmail.com)
 * @brief Inteface of module implementing Trie data structure.
 * 
 * Tries store key <-> value pairs and allows to perform match longest prefix
 * operation and key <-> value pair deletion and insertion.
 * 
 * @version 0.1
 * @date 2022-04-24
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#ifndef __COMPRESSED_TRIE_H__
#define __COMPRESSED_TRIE_H__
#include <stdbool.h>

/**
 * @brief Trie to store pairs: Key<String> <-> Key<Value>,
 *  only one value can be connected to given key. Keys needs to be numbers - strings
 *  which consists of digits.
 */
struct Trie;
typedef struct Trie Trie;

/** @brief Function inits compressed trie data structure and returns pointer to it.
 * 
 * @param memory_error : indicates if there was a memory error.
 * @return Trie* : created data structure.
 */
Trie *init_trie(bool *memory_error);

/**
 * @brief Function inserts key - value pair to the trie structure.
 *  If trie has already a value conntected to given key, it becomes overwritten.
 * 
 * Warning! Function doesn't check if given string are valid.
 * 
 * @param tree : pointer to trie in which we want to perform insertion.
 * @param key : String key (digit char string).
 * @param value : String value.
 * @return true : if insertion was successful.
 * @return false : if error has occured (structure state has not been modified).
 */
bool trie_insert(Trie *tree, const char *key, const char *value);

/**
 * @brief Returns value of the longest prefix of @p key that occurs in trie.
 * 
 * String key must consist of digits, eg. "8392"
 * If there is not such prefix, it returns NULL.
 * 
 * @param tree : trie to perform search in.
 * @param key : key to match.
 * @param memory_error : set to true if there was a memory error.
 * @return char* : found value.
 */
char *trie_match_longest_prefix(const Trie *tree, const char *key, bool *memory_error);

/**
 * @brief Removes all (key, value) pairs such that key has prefix which equals @p prefix.
 * 
 * @param tree : trie to remove data from.
 * @param prefix : prefix of key to delete.
 */
void trie_remove_subtree(Trie *tree, const char *prefix);

/**
 * @brief Deletes Trie data structure.
 * 
 * @param tree : Trie to delete.
 */
void trie_drop(Trie *tree);

void trie_debug_print(Trie *tree);

#endif /* __COMPRESSED_TRIE_H__ */