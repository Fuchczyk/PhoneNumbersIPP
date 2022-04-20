#ifndef __STRING_LIB_H__
#define __STRING_LIB_H__
#include <stdbool.h>
#include <stddef.h>

/** @brief Allocates memory and copies string content to it.
 * 
 * Provided string must end with '\0'.
 * Returns NULL if memory error has occured.
 * Programmer should free returned memory chunk.
 * 
 * @param to_clone : string to be cloned.
 * @return char* : pointer to cloned string.
 */
char *string_clone(const char *to_clone);

/** @brief Allocates memory and copies string content to it (from given index).
 * 
 * Provided string must end with '\0'.
 * Returns NULL if memory error has occured.
 * Programmer should free returned memory chunk.
 * 
 * @param to_clone : string to be cloned.
 * @param start_index : index of first char to be included in result string.
 * @return char* : pointer to cloned string.
 */
char *string_clone_from_index(const char *to_clone, size_t start_index);

/** @brief Shrinks given string.
 * 
 * Char of index @p cut_place is the first char 
 * NOT INCLUDED in given string.
 * 
 * If there was a memory error, memory chunk is not shrinked,
 * but provided string is shortened by NULL char.
 * 
 * @param string_to_cut : address of pointer of string to be cut.
 * @param cut_place : first index of char which should not be included.
 */
void string_cut_at_char(char **string_to_cut, size_t cut_place);

/** @brief Joins two string together by appending one to another.
 * 
 * Function does not perform free on @p to_append.
 * //TODO: Sprawdzić pisownię intact
 * If memory error has occured everything is intact.
 * 
 * @param to_extend : adress of pointer to string which should be extended.
 * @param to_append : pointer to string to be appended.
 * @return true : if operation was successful.
 * @return false : if there was a memory error.
 */
bool string_concat(char **to_extend, const char *to_append);

/** @brief Function checks if @p s2 is prefix of @p s1.
 * 
 * @param s1 : string to check prefix of.
 * @param start_char : index of char to start checking.
 * @param s2 : string which should be the prefix.
 * @param pref_len : length of the longest common prefix of @p s1 and @p s2.
 * @return true : if @s2 is the prefix of @p s1.
 * @return false : if @s2 is not the prefix of @p s1.
 */
bool string_check_prefixes(const char *s1, size_t start_char, const char *s2, size_t *pref_len);

#endif /* __STRING_LIB_H__ */