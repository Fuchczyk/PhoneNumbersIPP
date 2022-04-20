#include <stdio.h>
#include "compressed_trie.h"

int main(void) {
    bool e;
    Trie *t = init_trie(&e);

    trie_insert(t, "123", "5");
    trie_insert(t, "12356", "5");
    trie_insert(t, "12357", "7");
    
    trie_debug_print(t);

    trie_remove_subtree(t, "9");
    trie_drop(t);
    return 0;    
}