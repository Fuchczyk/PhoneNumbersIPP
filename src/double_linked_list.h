/**
 * @file double_linked_list.h
 * @author Przemysław Fuchs (pf438429@students.mimuw.edu.pl)
 * @brief Module implements double-linked list with element pointing.
 * @date 2022-05-30
 */
#ifndef __DOUBLE_LINKED_LIST_H__
#define __DOUBLE_LINKED_LIST_H__
#include "compressed_trie.h"
#include <stdbool.h>

/**
 * @brief Structure to represent list data structure.
 */
struct List;
/**
 * @brief Typedef to keep code clean and more readable.
 */
typedef struct List List;

/**
 * @brief Structure representing single element of the list.
 */
struct ListElement;
/**
 * @brief Typedef to keep code clean and more readable.
 */
typedef struct ListElement ListElement;

/**
 * @brief Strutcture to represent interator over the list.
 */
struct ListIterator;
/**
 * @brief Typedef to keep code clean and more readable.
 */
typedef struct ListIterator ListIterator;

/**
 * @brief Inits empty list data structure.
 *
 * @param[out] memory_error : set to true if memory error has occured.
 * @return List* : pointer to created empty list (NULL if memory_error).
 */
List *init_list(bool *memory_error);

/**
 * @brief Function inserts pointer into the list.
 *
 * Returned pointer lasts as long as inserted element is not removed from
 * the list or the list is dropped.
 *
 * Function makes copy of @p to_insert so no ownership is transferred.
 *
 * @param[in, out] list : pointer to list at which element @p to_insert is
 * inserted.
 * @param[in] to_insert : pointer to insert.
 * @return List* : pointer to inserted element (NULL if memory error occured).
 */
ListElement *list_insert(List *list, const char *to_insert);

/**
 * @brief Removes element from the list which lies behind given pointer.
 *
 * @param[in] element_to_remove : pointer to element to remove.
 */
void list_remove_ptr(ListElement *element_to_remove);

/**
 * @brief Drops list and all values that it holds.
 *
 * @param[in] to_drop : list to drop.
 */
void list_drop(List *to_drop);

/**
 * @brief Returns Node of the Trie which it corresponds to.
 *
 * Function should be used only if @p last_element is the only
 * element of its list, that is listelement_is_last(last_element) == true.
 *
 * @param[in] last_element : element to read correspondig TrieNode to.
 * @return TrieNode* : corresponding TrieNode.
 */
TrieNode *listelement_get_node(ListElement *last_element);

/**
 * @brief Creates iterator over given @p list.
 *
 * Iterator should be dropped by listiterator_drop() function.
 *
 * @param[in] list : list to create iterator of.
 * @param[out] memory_error : set to true if memory error has occured.
 * @return ListIterator* : created iterator.
 */
ListIterator *list_iterator(const List *list, bool *memory_error);

/**
 * @brief Checks if given iterator has at least one remaining element to visit.
 *
 * @param[in] iterator : iterator to check if there is an element to visit.
 * @return true : if iterator has element to visit.
 * @return false : if iterator has no elements to visit.
 */
bool listiterator_has_next(const ListIterator *iterator);

/**
 * @brief Checks if given list is empty.
 *
 * @param[in] list : list to check.
 * @return true : if list is empty.
 * @return false : if list is non-empty.
 */
bool list_isempty(const List *list);

/**
 * @brief Reads next element from the @p iterator.
 *
 * Function can be used if and only if iterator has next element.
 *
 * @param[in, out] iterator : iterator to get next element from.
 * @return const char* : value of next element.
 */
const char *listiterator_next(ListIterator *iterator);

/**
 * @brief Drops @p iterator.
 *
 * @param[in] iterator : iterator to drop.
 */
void listiterator_drop(ListIterator *iterator);

/**
 * @brief Checks if element of given pointer is last element of its List.
 *
 * @param[in] element : element to check if it is last element of it's list.
 * @return true : if element is last element.
 * @return false : if element's List contains at least two items.
 */
bool listelement_is_last(const ListElement *element);

/**
 * @brief Function sets @p connected_node as node connected to @p list.
 *
 * @param[out] list : list to set connected node of.
 * @param[in] connected_node : pointer to corresponding node.
 */
void list_set_node(List *list, TrieNode *connected_node);
#endif /* __DOUBLE_LINKED_LIST_H__ */