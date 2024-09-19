#include "common.h"
#include <stdlib.h>
#include <string.h>

bool string_empty(struct string *string) {
        return string->len == 0;
}

struct string string_one_char(char ch) {
        char* ptr = malloc(2);
        ptr[0] = ch;
        ptr[1] = '\0';
        return (struct string){.len = 1, .ptr = ptr};
}

struct string string_malloc_empty() {
        char* ptr = malloc(1);
        ptr[0] = '\0';
        return (struct string){.len = 0, .ptr = ptr};
}

// Creates a new var string with a initial capacity of 8 bytes.
struct var_string var_string_create() {
        char* ptr = malloc(8);

        if (ptr == NULL) {
                return (struct var_string){.ptr = NULL, .len = 0, .cap = 0,};
        }

        memset(ptr, 0, 8);
        return (struct var_string){
                .ptr = ptr,
                .len = 0,
                .cap = 8,
        };
}

bool var_string_valid(struct var_string *string) {
        return string->ptr == NULL;
}
