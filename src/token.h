#ifndef rob_token_h
#define rob_token_h

#include "rob_string.h"

#define RT(name) token_type_##name

enum token_type {
        RT(invalid),
        RT(eof),
        RT(equal),     // =
        RT(semicolon), // ;
        RT(minus),     // -
        RT(comma),     // ,
        RT(ident),     // test, bin, lib
        RT(string),    // "hello world"

        // Keywords
        RT(bin),
        RT(lib),
};

#undef RT

char *token_type_str(enum token_type token);

struct token {
        enum token_type type;
        struct string literal;
};


#endif // rob_token_h
