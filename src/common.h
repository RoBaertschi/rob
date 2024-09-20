#ifndef rob_common_h
#define rob_common_h

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "token.h"

#ifndef ASSERT

#include <assert.h>
#define ASSERT(expr) assert(expr)

#endif

typedef enum {
        result_ok,
        result_fopen_failed,
        result_alloc_failed,

        // lexer errors
        result_lexer_invalid_character,

        // parser errors
        result_parser_invalid_token,
        result_parser_unexpected_token,
} result_type;


struct error_context {
        int cur_errno;
        union {
                /// fopen failed
                char *filename;
                /// invalid character
                char invalid_ch;
                /// invalid token
                struct token invalid_token;
                struct { enum token_type found_token; enum token_type expected_token; } unexpected_token;
        };
};

struct result {
        result_type type;
        struct error_context ctx;
};

#define result_container(name, type)                                           \
        struct rc_##name {                                                     \
                struct result result;                                          \
                type ok;                                                       \
        }

#define OK_RESULT                                                              \
        (struct result) { .type = result_ok, .ctx = {0}, }

#define ALLOC_FAILED_RESULT                                                    \
        (struct result) { .type = result_alloc_failed, .ctx = {0}, }

#define TRY(res)                                                               \
        {                                                                      \
                struct result tmp_result = res;                                \
                if (tmp_result.type != result_ok) {                            \
                        return tmp_result;                                     \
                }                                                              \
        }

#define TRY_RC(res, rc_name)                                                   \
        {                                                                      \
                struct result tmp_result = res;                                \
                if (tmp_result.type != result_ok) {                            \
                        return (struct rc_name){.result = tmp_result,          \
                                                .ok = {0}};                    \
                }                                                              \
        }

#define GET_OR_RETURN(expr, var, var_type, return_type)                        \
        struct var_type var = {0};                                             \
        {                                                                      \
                struct rc_##var_type tmp_result = expr;                        \
                if (tmp_result.result.type != result_ok) {                     \
                        return (struct return_type){                           \
                            .result = tmp_result.result,                       \
                            .ok = {0},                                         \
                        };                                                     \
                }                                                              \
                var = tmp_result.ok;                                           \
        }

#define GET_EXSISTING_OR_RETURN(expr, var, var_type, return_type)              \
        {                                                                      \
                struct rc_##var_type tmp_result = expr;                        \
                if (tmp_result.result.type != result_ok) {                     \
                        return (struct return_type){                           \
                            .result = tmp_result.result,                       \
                            .ok = {0},                                         \
                        };                                                     \
                }                                                              \
                var = tmp_result.ok;                                           \
        }

#define TRY_AND_GET(res, var_name, var_type)                                   \
        {                                                                      \
                struct rc_##var_type tmp_rc = res;                             \
                if (tmp_rc.result.type != result_ok) {                     \
                        return tmp_rc.result;                                  \
                }                                                              \
                var_name = tmp_rc.ok;                                          \
        }

char *error_context_str(result_type type, struct error_context ctx);

#define rc_error_str(rc) error_context_str(rc.result.type, rc.result.ctx)


char *sprintf_alloc(char *format, ...);


result_container(token, struct token);
result_container(string, struct string);

#endif
