#ifndef __MEMORY_H__
#define __MEMORY_H__
#include <stdlib.h>

static inline void *wrap_malloc(size_t wanted_bytes) {
    return malloc(wanted_bytes);
}

static inline void *wrap_calloc(size_t members, size_t size_of_member) {
    return calloc(members, size_of_member);
}

static inline void *wrap_realloc(void *memory_chunk, size_t wanted_bytes) {
    return realloc(memory_chunk, wanted_bytes);
}

static inline void wrap_free(void *memory_chunk) {
    free(memory_chunk);
}

#endif /* __MEMORY_H__ */