#ifndef __COMPRESSED_TRIE_H__
#define __COMPRESSED_TRIE_H__
#include <stdbool.h>

struct Trie;
typedef struct Trie Trie;

/** @brief Function inits compressed trie data structure and returns pointer to it.
 * 
 * @param memory_error : indicates if there was a memory error.
 * @return Trie* : created data structure.
 */
Trie *init_trie(bool *memory_error);

bool trie_insert(Trie *tree, const char *key, const char *value);

const char *trie_get(const Trie *tree, const char *key);

void trie_remove(Trie *tree, const char *key);

void trie_drop(Trie *tree);

void trie_debug_print(Trie *tree);

#endif /* __COMPRESSED_TRIE_H__ */