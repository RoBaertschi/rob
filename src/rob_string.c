#include "rob_string.h"
#include <string.h>

struct string string_from_array(const char *array) {
        size_t len = strlen(array);
        char *ptr = malloc(len);
        memcpy(ptr, array, len);
        return (struct string){
            .len = len - 1,
            .ptr = ptr,
        };
}

struct string string_one_char(char ch) {
        char *ptr = malloc(2);
        ptr[0] = ch;
        ptr[1] = '\0';
        return (struct string){.len = 1, .ptr = ptr};
}

struct string string_malloc_empty(void) {
        char *ptr = malloc(1);
        ptr[0] = '\0';
        return (struct string){.len = 0, .ptr = ptr};
}
