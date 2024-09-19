#ifndef rob_common_h
#define rob_common_h

#include <stddef.h>
#include <stdbool.h>


struct string {
        char *ptr;
        /// Excludes \0
        size_t len;
};

bool string_empty(struct string *string);
struct string string_one_char(char ch);
struct string string_malloc_empty();

/// Does not maintain a \0, a string with \0 can be gotten with var_string_to_string.
/// A invalid string has ptr as NULL. Every function besides var_string_valid asserts "ptr != NULL".
struct var_string {
        char *ptr;
        size_t len;
        size_t cap;
};

struct var_string var_string_create();
bool var_string_valid(struct var_string *string);

#endif
