#include "double_linked_list.h"
#include "compressed_trie.h"
#include "memory.h"
#include <string.h>

/**
 * @brief Struct to manage structure of string list.
 */
struct List {
  ListElement *guard; ///< Pointer to the guard of the list.
};

/**
 * @brief Structure to represent element of the list.
 */
struct ListElement {
  char *value;           ///< Value of the element as a pointer to string.
  ListElement *previous; ///< Pointer to the previous element in the list.
  ListElement *next;     ///< Pointer to the next element in the list.
};

/**
 * @brief Struct to manage iteration at list values.
 */
struct ListIterator {
  ListElement *actual_element; ///< Pointer to the current element at which
                               ///< iterator points.
};

List *init_list(bool *memory_error) {
  List *list = wrap_malloc(sizeof(struct List));
  if (list == NULL) {
    *memory_error = true;
    return NULL;
  }

  ListElement *guard = wrap_malloc(sizeof(struct ListElement));
  if (guard == NULL) {
    wrap_free(list);
    *memory_error = true;
    return NULL;
  }

  guard->next = NULL;
  guard->previous = NULL;
  guard->value = NULL;

  list->guard = guard;

  return list;
}

ListElement *list_insert(List *list, const char *to_insert) {
  ListElement *element = wrap_malloc(sizeof(struct ListElement));
  if (element == NULL) {
    return NULL;
  }

  char *new_string = wrap_malloc(sizeof(char) * (strlen(to_insert) + 1));
  if (new_string == NULL) {
    wrap_free(element);
    return NULL;
  }
  strcpy(new_string, to_insert);

  ListElement *list_first_element = list->guard->next;

  element->previous = list->guard;
  element->value = new_string;
  element->next = list_first_element;

  if (list_first_element != NULL) {
    list_first_element->previous = element;
  }

  list->guard->next = element;

  return element;
}

void list_remove_ptr(ListElement *element_to_remove) {
  if (element_to_remove == NULL) {
    return;
  }

  ListElement *prev_element = element_to_remove->previous;
  ListElement *next_element = element_to_remove->next;

  prev_element->next = next_element;
  if (next_element != NULL) {
    next_element->previous = prev_element;
  }

  wrap_free(element_to_remove->value);
  wrap_free(element_to_remove);
}

void list_drop(List *to_drop) {
  if (to_drop == NULL) {
    return;
  }

  ListElement *node = to_drop->guard;

  while (node != NULL) {
    ListElement *to_delete = node;
    node = node->next;

    wrap_free(to_delete->value);
    wrap_free(to_delete);
  }

  wrap_free(to_drop);
}

ListIterator *list_iterator(const List *list, bool *memory_error) {
  ListIterator *iter = wrap_malloc(sizeof(struct ListIterator));
  if (iter == NULL) {
    *memory_error = true;
    return NULL;
  }

  iter->actual_element = list->guard->next;

  return iter;
}

const char *listiterator_next(ListIterator *iterator) {
  const char *value = iterator->actual_element->value;
  iterator->actual_element = iterator->actual_element->next;

  return value;
}

bool listiterator_has_next(const ListIterator *iterator) {
  return iterator->actual_element != NULL;
}

void listiterator_drop(ListIterator *iterator) { wrap_free(iterator); }

bool list_isempty(const List *list) { return list->guard->next == NULL; }

bool listelement_is_last(const ListElement *element) {
  if (element == NULL) {
    return false;
  }

  return (element->previous->value == NULL) && (element->next == NULL);
}

void list_set_node(List *list, TrieNode *connected_node) {
  list->guard->previous = (ListElement *)connected_node;
}

TrieNode *listelement_get_node(ListElement *last_element) {
  return (TrieNode *)last_element->previous->previous;
}