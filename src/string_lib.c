/**
 * @file string_lib.c
 * @author Przemysław Fuchs (pf438429@students.mimuw.edu.pl)
 * @brief Module implements advanced string operations.
 * @date 2022-05-07
 */
#include "string_lib.h"
#include "memory.h"
#include <string.h>

char *string_clone(const char *to_clone) {
  size_t to_clone_len = strlen(to_clone);

  char *result = wrap_malloc(sizeof(char) * (to_clone_len + 1));
  if (result == NULL) {
    return NULL;
  }

  strcpy(result, to_clone);

  return result;
}

char *string_clone_from_index(const char *to_clone, size_t start_index) {
  size_t to_clone_len = strlen(to_clone);

  char *result = wrap_malloc(sizeof(char) * (to_clone_len - start_index + 1));
  if (result == NULL) {
    return NULL;
  }

  strcpy(result, to_clone + start_index);

  return result;
}

void string_cut_at_char(char **string_to_cut, size_t cut_place) {
  char *tmp_ptr = wrap_realloc(*string_to_cut, sizeof(char) * (cut_place + 1));

  if (tmp_ptr == NULL) {
    (*string_to_cut)[cut_place] = '\0';
  } else {
    *string_to_cut = tmp_ptr;
    tmp_ptr[cut_place] = '\0';
  }
}

bool string_concat(char **to_extend, const char *to_append) {
  size_t to_extend_len = strlen(*to_extend);
  size_t res_len = to_extend_len + strlen(to_append);

  char *tmp_ptr = wrap_realloc(*to_extend, sizeof(char) * (res_len + 1));
  if (tmp_ptr == NULL) {
    return false;
  }

  *to_extend = tmp_ptr;
  strcpy(tmp_ptr + to_extend_len, to_append);

  return true;
}

bool string_check_prefixes(const char *s1, size_t start_char, const char *s2,
                           size_t *pref_len) {
  size_t length = 0;
  s1 = s1 + start_char;

  while (*s1 != '\0' && *s2 != '\0' && *s1 == *s2) {
    s1++;
    s2++;
    length++;
  }

  *pref_len = length;
  return (*s2 == '\0');
}