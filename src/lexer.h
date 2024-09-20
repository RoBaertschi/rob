#ifndef rob_lexer_h
#define rob_lexer_h

#include "common.h"
#include <stddef.h>
#include <stdio.h>


struct lexer {
        char ch;
        char read_ch;

        size_t col;
        size_t row;

        FILE *file;
};


result_container(lexer, struct lexer);

struct rc_lexer create_lexer(struct string file_name);
struct rc_token next_token(struct lexer *lexer);
void destroy_lexer(struct lexer *lexer);

#endif // rob_lexer_h
