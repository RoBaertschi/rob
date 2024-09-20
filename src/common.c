#include "common.h"
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *sprintf_alloc(char *format, ...) {
        va_list args;
        va_start(args, format);
        size_t needed = vsnprintf(NULL, 0, format, args);
        va_end(args);
        char *buffer = malloc(needed);
        va_start(args, format);
        vsprintf(buffer, format, args);
        va_end(args);
        return buffer;
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


