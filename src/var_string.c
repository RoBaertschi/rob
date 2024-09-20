#include "var_string.h"
#include <stdlib.h>
#include <string.h>

// Creates a new var string with a initial capacity of 8 bytes.
struct rc_var_string var_string_create(void) {
        char *ptr = malloc(8);

        if (ptr == NULL) {
                return (struct rc_var_string){.result = ALLOC_FAILED_RESULT,
                                              .ok = (struct var_string){
                                                  .ptr = NULL,
                                                  .len = 0,
                                                  .cap = 0,
                                              }};
        }

        memset(ptr, 0, 8);
        return (struct rc_var_string){.result = OK_RESULT,
                                      .ok = (struct var_string){
                                          .ptr = ptr,
                                          .len = 0,
                                          .cap = 8,
                                      }};
}

bool var_string_valid(struct var_string *string) { return string->ptr == NULL; }

static struct result __var_string_realloc(struct var_string *string,
                                          size_t new_length) {
        if (new_length <= string->cap) {
                return OK_RESULT;
        }

        string->ptr = realloc(string->ptr, new_length);
        string->cap = new_length;

        if (string->ptr == NULL) {
                return ALLOC_FAILED_RESULT;
        }

        return OK_RESULT;
}

static struct result __var_string_expand(struct var_string *string) {
        return __var_string_realloc(string, string->cap * 2);
}

static struct result __var_string_ensure_size(struct var_string *string,
                                              size_t new_size) {
        if (string->cap <= new_size) {
                TRY(__var_string_expand(string));
                return __var_string_ensure_size(string, new_size);
        }
        return OK_RESULT;
}

struct result var_string_append_c(struct var_string *string, char ch) {
        ASSERT(string->ptr != NULL && "string is invalid");

        size_t prev_len = string->len;

        TRY(__var_string_ensure_size(string, string->len + 1));

        string->ptr[prev_len] = ch;
        string->len++;

        return OK_RESULT;
}

struct rc_string var_string_to_string(struct var_string const *const string) {
        ASSERT(string->ptr != NULL && "string is invalid");

        char *ptr = malloc(string->len + /* \0 */ 1);

        if (ptr == NULL) {
                return (struct rc_string){
                    .ok = {0},
                    .result = ALLOC_FAILED_RESULT,
                };
        }

        memcpy(ptr, string->ptr, string->len);
        ptr[string->len] = '\0';

        return (struct rc_string){
            .ok =
                (struct string){
                    .ptr = ptr,
                    .len = string->len,
                },
            .result = OK_RESULT,
        };
}

void var_string_destroy(struct var_string *string) {
        free(string->ptr);
        // Ensure that the string is invalid
        string->ptr = NULL;
}
