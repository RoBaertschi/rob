#include "common.h"
#include "stb_ds.h"
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char **allocated_strings = NULL;

char *sprintf_alloc(char *format, ...) {
        va_list args;
        va_start(args, format);
        size_t needed = vsnprintf(NULL, 0, format, args);
        va_end(args);
        char *buffer = malloc(needed);
        va_start(args, format);
        vsprintf(buffer, format, args);
        va_end(args);
        arrput(allocated_strings, buffer);
        return buffer;
}

void free_all_strings(void) {
        for (ptrdiff_t i = 0; i < arrlen(allocated_strings); i++) {
                free(allocated_strings[i]);
        }
        arrfree(allocated_strings);
}

char *error_context_str(result_type type, struct error_context ctx) {
        switch (type) {
        case result_ok:
                return "ok";
        case result_fopen_failed:
                return sprintf_alloc("fopen failed with %s for file %s",
                                     strerror(ctx.cur_errno), ctx.filename);

        case result_alloc_failed:
                return sprintf_alloc("malloc failed with %d", ctx.cur_errno);
        case result_lexer_invalid_character:
                return sprintf_alloc(
                    "lexer found invalid character: '%c' or '%d'",
                    ctx.invalid_ch, ctx.invalid_ch);
        default:
                return "unkown error";
        }
}
