#include "phone_forward.h"
#include "compressed_trie.h"
#include "memory.h"

struct PhoneForward {
    Trie *database;
};

struct PhoneNumbers {
    char **numbers;
    size_t amount_of_numbers;
};

PhoneForward *phfwdNew(void) {
    PhoneForward *res = wrap_malloc(sizeof(struct PhoneForward));
    if (res == NULL) {
        return NULL;
    }

    bool memory_error = false;

    res->database = init_trie(&memory_error);
    if (memory_error) {
        wrap_free(res);
        return NULL;
    }

    return res;
}

void phfwdDelete(PhoneForward *pf) {
    trie_drop(pf->database);
    wrap_free(pf);
}

bool phfwdAdd(PhoneForward *pf, char const *num1, char const *num2) {
    trie_insert(pf->database, num1, num2);
}