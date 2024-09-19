#include "lexer.h"
#include "common.h"
#include <errno.h>
#include <stdio.h>

static void next_char(struct lexer *lexer) {
        lexer->ch = lexer->read_ch;
        lexer->read_ch = fgetc(lexer->file);

        // IMPORTANT: When we find a new line, we set col to 0.
        // Most editors seem to start
        // at col 1 in their files. The next time we call next_char,
        // we read over the new line
        // and set col to 1. So it will be correct at that point.
        if (lexer->ch == '\n') {
                lexer->row += 1;
                lexer->col = 0;
        } else {
                lexer->col += 1;
        }
}

struct create_lexer_result create_lexer(struct string file_name) {
        errno = 0;
        FILE *file = fopen(file_name.ptr, "rb");

        if (file == NULL) {
                return (struct create_lexer_result){
                    .type = rob_create_lexer_fopen_file_error,
                    .errno_result = errno,
                };
        }

        struct lexer lexer = {
            .ch = 0,
            .read_ch = 0,
            .col = 0,
            .row = 0,
            .file = file,
        };

        // Populates read_ch
        next_char(&lexer);
        // Populates ch
        next_char(&lexer);

        return (struct create_lexer_result){
            .type = rob_create_lexer_ok,
            .lexer = lexer,
        };
}

static bool is_digit(char ch) {
        return '0' <= ch && ch <= '9';
}

static bool is_valid_identifier_char(char ch) {
        return 'a' <= ch  && ch <= 'z' || 'A' <= ch && ch <= 'Z' || is_digit(ch);
}

static struct token read_identifier(struct lexer *lexer) {
        char buf[32] = {0};
        
        size_t length = 0;
        char cur_char = 0;

        while (next_char(lexer), is_valid_identifier_char(lexer->ch)) {
                
        }
}

struct next_token_result next_token(struct lexer *lexer) {
        struct token tok = {0};

        switch (lexer->ch) {
        case 0:
                tok.type = token_type_eof;
                tok.literal = string_malloc_empty();
                return (struct next_token_result){
                    .type = next_token_result_ok,
                    .token = tok,
                };
                break;
        case '=':
                tok.type = token_type_equal;
                tok.literal = string_one_char(lexer->ch);
                break;
        case ';':
                tok.type = token_type_semicolon;
                tok.literal = string_one_char(lexer->ch);
                break;
        case '-':
                tok.type = token_type_minus;
                tok.literal = string_one_char(lexer->ch);
                break;
        case ',':
                tok.type = token_type_comma;
                tok.literal = string_one_char(lexer->ch);
                break;
        case '"':
                // TODO: Parse string
                break;
        default:
                // TODO: Parse Identifier
                break;
        }

        next_char(lexer);

        return (struct next_token_result){.type = next_token_result_ok,
                                          .token = tok};
}

void destroy_lexer(struct lexer *lexer) { fclose(lexer->file); }
