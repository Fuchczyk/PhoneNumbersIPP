/**
 * @file blackred_tree.h
 * @author Przemysław Fuchs (pf438429@students.mimuw.edu.pl)
 * @brief Inteface of module implementing Black-Red tree set structure.
 *
 * Keys should consist of digits and "*" and "#" symbols.
 *
 * @date 2022-05-22
 */
#ifndef __BLACKRED_TREE_H__
#define __BLACKRED_TREE_H__
#include "dynamic_array.h"
#include <stdbool.h>

/**
 * @brief BRTree set data structure.
 */
struct BRTree;

/**
 * @brief Typedef to make code more clean and to improve readability.
 */
typedef struct BRTree BRTree;

/**
 * @brief Inits empty BlackRed Tree structure.
 *
 * @param[out] memory_error : set to true if memory error has occured.
 * @return BRTree* : empty set (NULL if memory error has occured).
 */
BRTree *init_tree(bool *memory_error);

/**
 * @brief Inserts a new key into BlackRed Tree structure.
 *
 * Warning! Function does NOT copy content of @p to_insert.
 *
 * @param[in, out] tree : tree to insert key into.
 * @param[in] to_insert : string of digits to insert.
 * @return true : if insertion was successful.
 * @return false : if insertion has failed.
 */
bool brtree_insert(BRTree *tree, char *to_insert);

/**
 * @brief Removes given set and return its representation as DynamicArray.
 *
 * Array is sorted lexicographiclly.
 *
 * @param[in] tree : set to convert.
 * @param[out] memory_error : set to true if memory error has occured.
 * @return DynamicArray* : array of keys sorted lexicographiclly (NULL if memory
 * error has occured).
 */
DynamicArray *brtree_conversion(BRTree *tree, bool *memory_error);

/**
 * @brief Drops BlackRed Tree structure and all stored strings.
 *
 * @param[in] tree : Tree to drop.
 */
void brtree_drop(BRTree *tree);
#endif /* __BLACKRED_TREE_H__ */