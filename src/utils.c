#include "utils.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>


static struct {
    char** items;
    size_t count;
    size_t capacity;
} sp = {
    .items = NULL,
    .count = 0,
    .capacity = 0,
};

void sp_init() {
    da_ensure_size(&sp, 30, sizeof(char*));
}

void sp_destroy() {
    da_foreach(&sp, char*, str) {
        free(str);
    }
    free(sp.items);
}

char* s(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    size_t needed = vsnprintf(NULL, 0, fmt, args);
    va_end(args);
    char *buffer = malloc(needed);
    va_start(args, fmt);
    vsprintf(buffer, fmt, args);
    va_end(args);
    da_append(&sp, buffer);
    return buffer;
}

