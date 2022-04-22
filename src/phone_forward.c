#include "phone_forward.h"
#include "compressed_trie.h"
#include "memory.h"
#include <string.h>
#include <assert.h>
#include <ctype.h>

struct PhoneForward {
    Trie *database;
};

struct PhoneNumbers {
    char **numbers;
    size_t amount_of_numbers;
};

static bool verify_number(const char *num) {
    while (*num != '\0') {
        if (!isdigit(*num)) {
            return false;
        }

        num++;
    }

    return num;
}

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
    if (pf == NULL) {
        return;
    }

    trie_drop(pf->database);
    wrap_free(pf);
}

bool phfwdAdd(PhoneForward *pf, char const *num1, char const *num2) {
    if (!verify_number(num1) || !verify_number(num2)) {
        return false;
    }

    return trie_insert(pf->database, num1, num2);
}

void phfwdRemove(PhoneForward *pf, char const *num) {
    if (num == NULL) {
        return;
    }
    
    if (!verify_number(num) || strlen(num) == 0) {
        return;
    }

    //trie_debug_print(pf->database);
    trie_remove_subtree(pf->database, num);
}

PhoneNumbers * phfwdGet(PhoneForward const *pf, char const *num){
    PhoneNumbers *result = wrap_malloc(sizeof(struct PhoneNumbers));
    if (result == NULL) {
        return NULL;
    }

    if (!verify_number(num)) {
        result->amount_of_numbers = 0;
        result->numbers = NULL;
        return result;
    }

    bool mem_error = false;

    char *forwarding = trie_match_longest_prefix(pf->database, num, &mem_error);
    if (forwarding == NULL) {
        forwarding = malloc(sizeof(char) * (strlen(num) + 1));
        if (forwarding == NULL) {
            wrap_free(result);
            return NULL;
        }

        strcpy(forwarding, num);
    }

    if (mem_error) {
        wrap_free(result);
        return NULL;
    }

    result->numbers = wrap_malloc(sizeof(char *) * 1);
    if (result->numbers == NULL) {
        wrap_free(result);
        wrap_free(forwarding);
        return NULL;
    }

    result->numbers[0] = forwarding;
    result->amount_of_numbers = 1;

    return result;
}

void phnumDelete(PhoneNumbers *pnum) {
    if (pnum == NULL) {
        return;
    }

    for (size_t index = 0 ; index < pnum->amount_of_numbers ; index++) {
        wrap_free(pnum->numbers[index]);
    }

    wrap_free(pnum->numbers);
    wrap_free(pnum);
}

char const * phnumGet(PhoneNumbers const *pnum, size_t idx) {
    if (pnum == NULL) {
        return NULL;
    }

    if (idx >= pnum->amount_of_numbers) {
        return NULL;
    }

    return pnum->numbers[idx];
}

PhoneNumbers * phfwdReverse(PhoneForward const *pf, char const *num) {
    assert(false);
}

