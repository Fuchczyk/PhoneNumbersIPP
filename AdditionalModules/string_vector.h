#ifndef __STRING_VECTOR_H__
#define __STRING_VECTOR_H__
#include <stdbool.h>
#include <stddef.h>

struct StringVector;
typedef struct StringVector StringVector;

StringVector *init_stringvector(void);

bool stringvector_add(StringVector **vector, const char *to_add);

void stringvector_remove(StringVector *vector, const char *to_remove);

size_t stringvector_size(const StringVector *vector);

void stringvector_drop(StringVector *to_drop);

void stringvector_debug(const StringVector *to_debug);

#endif /* __STRING_VECTOR_H__ */