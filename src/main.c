#include <stdio.h>
#include "compressed_trie.h"

int main(void) {
    bool e;
    Trie *t = init_trie(&e);

    trie_insert(t, "123", "1234");
    trie_insert(t, "12", "5");
    
    trie_debug_print(t);
    trie_drop(t);
    return 0;    
}