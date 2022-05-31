/**
 * @file dynamic_array.h
 * @author Przemysław Fuchs (pf438429@students.mimuw.edu.pl)
 * @brief Module implement dynamic container that is able to store items.
 * @date 2022-05-30
 */
#ifndef __DYNAMIC_ARRAY_H__
#define __DYNAMIC_ARRAY_H__
#include <stdbool.h>
#include <stddef.h>

/**
 * @brief Structure to store items.
 */
struct DynamicArray;
/**
 * @brief Typedef to keep code more clean and readable.
 */
typedef struct DynamicArray DynamicArray;

/**
 * @brief Inits new DynamicArray data structure.
 *
 * @param[out] memory_error : set to true if memory error has occured.
 * @return DynamicArray* : empty array (NULL if memory error).
 */
DynamicArray *init_darray(bool *memory_error);

/**
 * @brief Appends new item at the end of array.
 *
 * @param[in, out] dynamic_array : array to append item to.
 * @param[in] to_push : item (as void *) to push.
 * @param[out] memory_error : set to true if memory error has occured.
 */
void darray_push(DynamicArray *dynamic_array, void *to_push,
                 bool *memory_error);

/**
 * @brief Reads number of elements stored in data structure.
 *
 * @param[in] dynamic_array : array to check size of.
 * @return size_t : number of elements stored in @p dynamic_array.
 */
size_t darray_size(const DynamicArray *dynamic_array);

/**
 * @brief Performs conversion of structre to the array of inserted items.
 *
 * Structure pointed by @p dynamic_array is dropped and can't be used later on.
 * User should perform stdlib's free on returned value.
 *
 * @param[in] dynamic_array : array to convert.
 * @return void** : result of conversion.
 */
void **darray_convert(DynamicArray *dynamic_array);

/**
 * @brief Temporaily allows user to look into array's items.
 *
 * Warning! Returned pointer is only valid as long as no operation is done
 * on the @p dynamic_array.
 *
 * @param[in] dynamic_array : array to lookup.
 * @return const void** : array represenation of @p dynamic_array.
 */
const void **darray_temporary_lookup(DynamicArray *dynamic_array);
#endif /* __DYNAMIC_ARRAY_H__ */