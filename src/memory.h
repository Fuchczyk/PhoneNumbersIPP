#ifndef __MEMORY_H__
#define __MEMORY_H__
#include <stdlib.h>

/**
 * @brief Function to wrap allocation by malloc.
 * 
 * @param wanted_bytes : bytes of block to allocate.
 * @return void* : pointer to beggining of the block, allocated by malloc.
 */
static inline void *wrap_malloc(size_t wanted_bytes) {
    return malloc(wanted_bytes);
}

/**
 * @brief Function to wrap allocation by calloc.
 * 
 * @param members : number of objects to allocate.
 * @param size_of_member : size of one object in bytes.
 * @return void* : pointer to beggining of allocated memory block.
 */
static inline void *wrap_calloc(size_t members, size_t size_of_member) {
    return calloc(members, size_of_member);
}

/**
 * @brief Function to wrap reallocation by realloc.
 * 
 * @param memory_chunk : pointer to beggining of memory chunk to reallocate.
 * @param wanted_bytes : wanted size of new memory chunk.
 * @return void* : result of realloc() function.
 */
static inline void *wrap_realloc(void *memory_chunk, size_t wanted_bytes) {
    return realloc(memory_chunk, wanted_bytes);
}

/**
 * @brief Function to wrap memory release by free.
 * 
 * @param memory_chunk : pointer to beggining of memory chunk to free.
 */
static inline void wrap_free(void *memory_chunk) {
    free(memory_chunk);
}

#endif /* __MEMORY_H__ */