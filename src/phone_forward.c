/**
 * @file phone_forward.c
 * @author Przemysław Fuchs (pf438429@students.mimuw.edu.pl)
 * @brief Module implements interface specified in phone_forward.h
 * @date 2022-05-07
 */
#include "phone_forward.h"
#include "blackred_tree.h"
#include "compressed_trie.h"
#include "double_linked_list.h"
#include "memory.h"
#include <assert.h>
#include <ctype.h>
#include <string.h>

/**
 * @brief Preprocessor macro to make some parameter marked as intentionally
 * unused.
 */
#define UNUSED(X) (void)X;

/**
 * @brief Struct visible to library user which is wrapper for trie structure.
 */
struct PhoneForward {
  Trie *database_forward; ///< Trie to store forwards in.
  Trie *database_reverse; ///< Trie to store reverses in.
  List *fresh_list; ///< Fresh list to use in functions in case of memory error.
};

/**
 * @brief Structure to manage getting information about phone forwarding.
 */
struct PhoneNumbers {
  char **numbers;           ///< Array of strings representing numbers.
  size_t amount_of_numbers; ///< Number of elements in array.
};

/**
 * @brief Struct to store pair of values about number foward.
 */
struct ForwardRecord {
  char *forwarding; ///< Number as value of forwarding.
  ListElement
      *reverse_record; ///< Pointer to element representing reverse forward.
};

/**
 * @brief Typedef to keep code clean and more readable.
 */
typedef struct ForwardRecord ForwardRecord;

/**
 * @brief Checks if given char is digit (defined as in the given documentation).
 *
 * @param to_check : character to check.
 * @return true : if @p to_check is digit.
 * @return false : if @p to_check isn't digit.
 */
static inline bool is_digit(char to_check) {
  return (to_check == '*') || (to_check == '#') || isdigit(to_check);
}

/**
 * @brief Function verifies if string privided by the user is valid phone
 * number.
 *
 * @param[in] num : number to verify.
 * @return true : if string is correct phone number.
 * @return false : if string does not represent phone number.
 */
static bool verify_number(const char *num) {
  if (num == NULL) {
    return false;
  }

  while (*num != '\0') {
    if (!is_digit(*num)) {
      return false;
    }

    num++;
  }

  return true;
}

/**
 * @brief Function serves as free_function to init Trie data structure with
 * values as ForwardRecord representing forwarding.
 *
 * It assures that reverse forwarding are also deleted from other Trie, which
 * stores forwarding reverses.
 *
 * If key is NULL then function only frees stored value.
 * If key and value are NULLs then fuction does nothing.
 *
 * @param[in] value : value of the node being deleted.
 * @param[in] key : key corresponding to the @p value.
 * @param[out] other_configuration : pointer to the Trie representing
 * reverse-forwarding database.
 */
static void string_free_wrapper(void *value, const char *key,
                                void *other_configuration) {
  if (key != NULL && value != NULL) {
    Trie *database_reverse = (Trie *)other_configuration;
    ListElement *rev_element = ((ForwardRecord *)value)->reverse_record;

    if (listelement_is_last(rev_element)) {
      TrieNode *node = listelement_get_node(rev_element);
      trie_remove_from_ptr(database_reverse, node, key);
    } else {
      list_remove_ptr(rev_element);
    }
  }

  if (value != NULL) {
    wrap_free(((ForwardRecord *)value)->forwarding);
    wrap_free(value);
  }
}

/**
 * @brief Function serves as free function for Trie with values as List.
 *
 * @param[in] value : value (list) to delete.
 * @param[in] key : key corresponding to value being deleted.
 * @param[in] other_configuration : nothing is required.
 */
static void linkedlist_free_wrapper(void *value, const char *key,
                                    void *other_configuration) {
  UNUSED(key);
  UNUSED(other_configuration);

  list_drop((List *)value);
}

/**
 * @brief Function inserts reverse record to the database.
 *
 * @param[in, out] pf : structure to insert reversion into.
 * @param[in] key : @p num1 used at phfwdAdd.
 * @param[in] value : @p num2 used at phfwdAdd.
 * @param[out] save_list : ForwardRecord to save pointer to inserted List
 * element.
 * @return true : if insertion was successful.
 * @return false : if insertion has failed (nothing changes).
 */
static bool reverse_insert(PhoneForward *pf, const char *key, const char *value,
                           ForwardRecord *save_list) {
  bool memory_error = false;
  if (pf->fresh_list == NULL) {
    pf->fresh_list = init_list(&memory_error);
    if (memory_error) {
      return false;
    }
  }

  TrieNode *located_node;
  List *reverse_list = (List *)trie_locate_node(pf->database_reverse, value,
                                                pf->fresh_list, &located_node);
  if (reverse_list == NULL) {
    return false;
  } else if (reverse_list == pf->fresh_list) {
    list_set_node(reverse_list, located_node);

    pf->fresh_list = init_list(&memory_error);
    if (memory_error) {
      pf->fresh_list = NULL;
    }
  }

  ListElement *inserted_element = list_insert(reverse_list, key);
  if (inserted_element == NULL) {
    if (list_isempty(reverse_list)) {
      trie_remove_from_ptr(pf->database_reverse, located_node, key);
    }
    return false;
  }

  save_list->reverse_record = inserted_element;
  return true;

  /*StringTable *reverse_table = (StringTable *)
  trie_locate_node(pf->database_reverse, value, pf->fresh_hashtable); if
  (reverse_table == NULL) { return false; } else if (reverse_table ==
  pf->fresh_hashtable) { pf->fresh_hashtable = init_stringtable(&memory_error);
    if (memory_error) {
      pf->fresh_hashtable = NULL;
    }
  }

  if (!stringtable_insert(reverse_table, key, NULL)) {
    return false;
  }

  return true;*/
}

PhoneForward *phfwdNew(void) {
  PhoneForward *res = wrap_malloc(sizeof(struct PhoneForward));
  if (res == NULL) {
    return NULL;
  }

  bool memory_error = false;

  res->database_reverse =
      init_trie(&memory_error, linkedlist_free_wrapper, NULL);
  if (memory_error) {
    wrap_free(res);
    return NULL;
  }

  res->database_forward =
      init_trie(&memory_error, string_free_wrapper, res->database_reverse);
  if (memory_error) {
    trie_drop(res->database_reverse);
    wrap_free(res);
    return NULL;
  }

  res->fresh_list = init_list(&memory_error);
  if (memory_error) {
    trie_drop(res->database_forward);
    trie_drop(res->database_reverse);
    wrap_free(res);
    return NULL;
  }

  return res;
}

void phfwdDelete(PhoneForward *pf) {
  if (pf == NULL) {
    return;
  }

  trie_drop(pf->database_forward);
  trie_drop(pf->database_reverse);

  list_drop(pf->fresh_list);

  wrap_free(pf);
}

bool phfwdAdd(PhoneForward *pf, char const *num1, char const *num2) {
  if (pf == NULL) {
    return false;
  }

  if (num1 == NULL || num2 == NULL) {
    return false;
  }

  if (!verify_number(num1) || !verify_number(num2)) {
    return false;
  }

  if (strlen(num1) == 0 || strlen(num2) == 0) {
    return false;
  }

  if (strcmp(num1, num2) == 0) {
    return false;
  }

  char *inserted_value = wrap_malloc(sizeof(char) * (strlen(num2) + 1));
  if (inserted_value == NULL) {
    return false;
  }
  strcpy(inserted_value, num2);

  ForwardRecord *record = wrap_malloc(sizeof(struct ForwardRecord));
  if (record == NULL) {
    wrap_free(inserted_value);
    return false;
  }

  record->forwarding = inserted_value;
  record->reverse_record = NULL;

  TrieNode *inserted_node = trie_insert(pf->database_forward, num1, record);

  if (inserted_node == NULL) {
    wrap_free(record->forwarding);
    wrap_free(record);
    return false;
  }

  if (!reverse_insert(pf, num1, num2, record)) {
    trie_remove_from_ptr(pf->database_forward, inserted_node, num1);

    return false;
  }

  return true;
}

void phfwdRemove(PhoneForward *pf, char const *num) {
  if (num == NULL || pf == NULL) {
    return;
  }

  if (!verify_number(num) || strlen(num) == 0) {
    return;
  }

  trie_remove_subtree(pf->database_forward, num);
}

PhoneNumbers *phfwdGet(PhoneForward const *pf, char const *num) {
  if (pf == NULL) {
    return NULL;
  }

  PhoneNumbers *result = wrap_malloc(sizeof(struct PhoneNumbers));
  if (result == NULL) {
    return NULL;
  }

  if (pf == NULL || num == NULL || !verify_number(num) || strlen(num) == 0) {
    result->amount_of_numbers = 0;
    result->numbers = NULL;
    return result;
  }

  bool mem_error = false;
  size_t prefix_length = 0;

  ForwardRecord *forwarding =
      trie_match_longest_prefix(pf->database_forward, num, &prefix_length);
  if (mem_error) {
    wrap_free(result);
    return NULL;
  }

  result->numbers = wrap_malloc(sizeof(char *) * 1);
  if (result->numbers == NULL) {
    wrap_free(result);
    return NULL;
  }

  if (forwarding == NULL) {
    result->numbers[0] = wrap_malloc(sizeof(char) * (strlen(num) + 1));
    if (result->numbers[0] == NULL) {
      wrap_free(result->numbers);
      wrap_free(result);
      return NULL;
    }

    strcpy(result->numbers[0], num);
  } else {
    size_t forward_len = strlen(forwarding->forwarding);
    size_t res_len = forward_len + (strlen(num) - prefix_length);

    result->numbers[0] = wrap_malloc(sizeof(char) * (res_len + 1));
    if (result->numbers[0] == NULL) {
      wrap_free(result->numbers);
      wrap_free(result);
      return NULL;
    }

    memcpy(result->numbers[0], forwarding->forwarding,
           forward_len * sizeof(char));
    strcpy(result->numbers[0] + forward_len, num + prefix_length);
  }
  result->amount_of_numbers = 1;

  return result;
}

void phnumDelete(PhoneNumbers *pnum) {
  if (pnum == NULL) {
    return;
  }

  for (size_t index = 0; index < pnum->amount_of_numbers; index++) {
    wrap_free(pnum->numbers[index]);
  }

  wrap_free(pnum->numbers);
  wrap_free(pnum);
}

char const *phnumGet(PhoneNumbers const *pnum, size_t idx) {
  if (pnum == NULL) {
    return NULL;
  }

  if (idx >= pnum->amount_of_numbers) {
    return NULL;
  }

  return pnum->numbers[idx];
}

/**
 * @brief Function sorts elements from @p to_prepare lexicographiclly and
 *  removes multiple values from it.
 *
 * @param[in] to_prepare : dynamic array to take values from.
 * @param[in] res_num : string to insert into results array.
 * @return DynamicArray* : prepared array.
 */
static DynamicArray *prepare_reverses(DynamicArray *to_prepare,
                                      const char *res_num) {
  bool memory_error = false;

  size_t rev_size = darray_size(to_prepare);
  char **rev_results = (char **)darray_convert(to_prepare);

  BRTree *new_results = init_tree(&memory_error);
  if (memory_error) {
    for (size_t ind = 0; ind < rev_size; ind++) {
      wrap_free(rev_results[ind]);
    }

    wrap_free(rev_results);
    return NULL;
  }

  for (size_t ind = 0; ind < rev_size; ind++) {
    if (!brtree_insert(new_results, rev_results[ind])) {
      brtree_drop(new_results);
      for (size_t ind_new = 0; ind_new < rev_size; ind_new++) {
        wrap_free(rev_results[ind_new]);
      }

      wrap_free(rev_results);
      return NULL;
    }
  }

  char *cpy = wrap_malloc(sizeof(char) * (strlen(res_num) + 1));
  if (cpy == NULL) {
    wrap_free(rev_results);
    brtree_drop(new_results);

    return NULL;
  }

  strcpy(cpy, res_num);

  if (!brtree_insert(new_results, cpy)) {
    wrap_free(rev_results);
    wrap_free(cpy);
    brtree_drop(new_results);

    return NULL;
  }

  wrap_free(rev_results);

  DynamicArray *da = brtree_conversion(new_results, &memory_error);
  if (memory_error) {
    return NULL;
  }

  return da;
}

PhoneNumbers *phfwdReverse(PhoneForward const *pf, char const *num) {
  if (pf == NULL) {
    return NULL;
  }

  if (pf == NULL || num == NULL || !verify_number(num) || strlen(num) == 0) {
    PhoneNumbers *result = wrap_malloc(sizeof(struct PhoneNumbers));
    if (result == NULL) {
      return NULL;
    }

    result->amount_of_numbers = 0;
    result->numbers = NULL;
    return result;
  }

  DynamicArray *array = trie_traverse_down(pf->database_reverse, num);
  if (array == NULL) {
    return NULL;
  }

  DynamicArray *checked_array = prepare_reverses(array, num);
  if (checked_array == NULL) {
    return NULL;
  }

  size_t res_size = darray_size(checked_array);
  char **res_array = (char **)darray_convert(checked_array);

  /*debugx(res_array, res_size);
  qsort(res_array, res_size, sizeof(char *), comparator);
  debugx(res_array, res_size);*/

  PhoneNumbers *res = wrap_malloc(sizeof(struct PhoneNumbers));
  if (res == NULL) {
    for (size_t ind = 0; ind < res_size; ind++) {
      wrap_free(res_array[ind]);
    }

    wrap_free(res_array);
    return NULL;
  }

  res->amount_of_numbers = res_size;
  res->numbers = res_array;

  return res;
}
