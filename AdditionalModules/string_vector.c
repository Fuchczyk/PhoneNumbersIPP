/**
 * @file string_vector.c
 * @author Przemysław Fuchs (fuchs.przemyslaw@gmail.com)
 * @brief Module implements vector of strings (char pointers).
 * @version 0.1
 * @date 2022-04-24
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include "string_vector.h"
#include "string_lib.h"
#include "memory.h"
#include <string.h>

#define VECTOR_START_SIZE 16

/**
 * @brief Structure to manage dynamic sized array of strings.
 */
struct StringVector {
    char **array; ///< Array of strings (stored data).

    size_t max_elements; ///< Maximum number of stored elements.
    size_t first_free_index; ///< Index of first free field in array.
};

static StringVector *init_stringvector_nonempty(bool *error_occured) {
    StringVector *inited = wrap_malloc(sizeof(struct StringVector));
    if (inited == NULL) {
        *error_occured = true;
        return NULL;
    }

    inited->array = wrap_malloc(sizeof(char *) * VECTOR_START_SIZE);
    if (inited->array == NULL) {
        wrap_free(inited);
        *error_occured = true;
        return NULL;
    }

    inited->first_free_index = 0;
    inited->max_elements = VECTOR_START_SIZE;

    return inited;
}

static bool stringvector_extend(StringVector *vector) {
    size_t wanted_size = vector->max_elements * 2;
    char **realloced = wrap_realloc(vector->array, sizeof(char *) * wanted_size);

    if (realloced == NULL) {
        return false;
    }

    vector->array = realloced;
    vector->max_elements = wanted_size;

    return true;
}

static bool stringvector_shrink(StringVector *vector) {
    size_t wanted_size = vector->max_elements / 2;
    char **realloced = wrap_realloc(vector->array, sizeof(char *) * wanted_size);

    if (realloced == NULL) {
        return false;
    }

    vector->array = realloced;
    vector->max_elements = wanted_size;

    return true;
}

static bool stringvector_add_consume(StringVector **vector, char *to_add) {
    bool error_occured = false;
    if (*vector == NULL) {
        *vector = init_stringvector_nonempty(&error_occured);
        if (error_occured) {
            return false;
        }
    }

    if ((*vector)->first_free_index == (*vector)->max_elements) {
        error_occured = stringvector_extend(*vector);
        if (error_occured) {
            return false;
        }
    }

    size_t index = (*vector)->first_free_index;
    (*vector)->first_free_index += 1;
    (*vector)->array[index] = to_add;

    return true;
}

StringVector *init_stringvector(void) {
    return NULL;
}

bool stringvector_add(StringVector **vector, const char *to_add) {
    bool error_occured = false;

    char *to_add_clone = string_clone(to_add);
    if (to_add_clone == NULL) {
        return false;
    }

    return stringvector_add_consume(vector, to_add_clone);
}

void stringvector_remove(StringVector *vector, const char *to_remove) {
    for (size_t index = 0 ; index < vector->first_free_index ; vector++) {
        if (strcmp(to_remove, vector->array[index]) == 0) {
            wrap_free(vector->array[index]);
            
            vector->array[index] = vector->array[vector->first_free_index - 1];
            vector->first_free_index -= 1;
            break;
        }
    }

    if (vector->first_free_index == vector->max_elements / 4) {
        stringvector_shrink(vector);
    }
}

size_t stringvector_size(const StringVector *vector) {
    return vector->first_free_index;
}

void stringvector_drop(StringVector *to_drop) {
    if (to_drop == NULL) {
        return;
    }

    for (size_t index = 0 ; index < to_drop->first_free_index ; index++) {
        wrap_free(to_drop->array[index]);
    }

    wrap_free(to_drop->array);
    wrap_free(to_drop);
}