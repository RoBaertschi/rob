#ifndef rob_var_string_h
#define rob_var_string_h
#include <stddef.h>
#include <stdbool.h>
#include "common.h"

/// Does not maintain a \0, a string with \0 can be gotten with
/// var_string_to_string. A invalid string has ptr as NULL. Every function
/// besides var_string_valid asserts "ptr != NULL".
struct var_string {
        char *ptr;
        size_t len;
        size_t cap;
};

result_container(var_string, struct var_string);

struct rc_var_string var_string_create(void);

bool var_string_valid(struct var_string *string);
struct result var_string_append_c(struct var_string *string, char ch);

struct rc_string var_string_to_string(struct var_string const *const string);
void var_string_destroy(struct var_string *string);

#endif // rob_var_string_h
