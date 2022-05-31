/**
 * @file dynamic_array.c
 * @author Przemysław Fuchs (pf438429@students.mimuw.edu.pl)
 * @brief Module implements interface presented in dynamic_array.h.
 * @date 2022-05-30
 */
#include "dynamic_array.h"
#include "memory.h"

/**
 * @brief Defines size of freshly created array.
 */
#define BASE_SIZE 8

/**
 * @brief Struct to manage variable-size array to store arbitrary elements as
 *  void pointers.
 */
struct DynamicArray {
  void **array;    ///< Array to store elements in.
  size_t capacity; ///< Maximal number of elements in array (maximum_index - 1).
  size_t elements; ///< Actual number of elements in structure.
};

/**
 * @brief Function extends capacity of given array twice.
 *
 * @param to_extend : array that should be extended.
 * @return true : if array was extended successfully.
 * @return false : if some kind of error has occured during array extending.
 */
static bool darray_extend(DynamicArray *to_extend) {
  size_t result_size = to_extend->capacity * 2;

  void **new_array =
      wrap_realloc(to_extend->array, sizeof(void *) * result_size);
  if (new_array == NULL) {
    return false;
  }

  to_extend->array = new_array;
  to_extend->capacity = result_size;

  return true;
}

DynamicArray *init_darray(bool *memory_error) {
  DynamicArray *created = wrap_malloc(sizeof(struct DynamicArray));
  if (created == NULL) {
    *memory_error = true;
    return NULL;
  }

  created->array = wrap_malloc(sizeof(void *) * BASE_SIZE);
  if (created->array == NULL) {
    *memory_error = true;

    wrap_free(created);
    return NULL;
  }

  created->capacity = BASE_SIZE;
  created->elements = 0;

  return created;
}

void darray_push(DynamicArray *dynamic_array, void *to_push,
                 bool *memory_error) {
  if (dynamic_array->capacity == dynamic_array->elements) {
    if (!darray_extend(dynamic_array)) {
      *memory_error = true;
      return;
    }
  }

  dynamic_array->array[dynamic_array->elements] = to_push;
  dynamic_array->elements++;
}

size_t darray_size(const DynamicArray *dynamic_array) {
  return dynamic_array->elements;
}

void **darray_convert(DynamicArray *dynamic_array) {
  void **result_array = dynamic_array->array;

  wrap_free(dynamic_array);
  return result_array;
}

const void **darray_temporary_lookup(DynamicArray *dynamic_array) {
  return (const void **)dynamic_array->array;
}