#ifndef rob_string_h
#define rob_string_h

#include <stdbool.h>
#include <stdlib.h>

struct string {
        char *ptr;
        /// Excludes \0
        size_t len;
};


bool string_empty(struct string *string);
struct string string_one_char(char ch);
struct string string_malloc_empty(void);
struct string string_from_array(const char *array);

#endif // rob_string_h
