#ifndef rob_lexer_h
#define rob_lexer_h

#include "common.h"
#include <stddef.h>
#include <stdio.h>


#define RT(name) token_type_##name

enum token_type {
        RT(eof),
        RT(equal),     // =
        RT(semicolon), // ;
        RT(minus),     // -
        RT(comma),     // ,
        RT(ident),     // test, bin, lib
        RT(string),    // "hello world"
};

#undef RT

struct token {
        enum token_type type;
        struct string literal;
};

struct lexer {
        char ch;
        char read_ch;

        size_t col;
        size_t row;

        FILE *file;
};

enum next_token_result_type {
        next_token_result_ok,
        next_token_result_invalid_token,
};

struct next_token_result {
        enum next_token_result_type type;
        struct token token;
};

struct create_lexer_result {
        enum {
                rob_create_lexer_ok,
                rob_create_lexer_fopen_file_error,
        } type;
        union {
                struct lexer lexer;
                /// The errno value after the failed fopen
                int errno_result;
        };
};

struct create_lexer_result create_lexer(struct string file_name);
struct next_token_result next_token(struct lexer *lexer);
void destroy_lexer(struct lexer *lexer);

#endif // rob_lexer_h
