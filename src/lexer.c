#include "lexer.h"
#include "common.h"
#include "var_string.h"
#include <errno.h>
#include <stdio.h>
#include <string.h>


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

struct rc_lexer create_lexer(struct string file_name) {
        errno = 0;
        FILE *file = fopen(file_name.ptr, "rb");

        if (file == NULL) {
                return (struct rc_lexer){.ok = {0},
                                         .result = {
                                             .type = result_fopen_failed,
                                             .ctx =
                                                 (struct error_context){
                                                     .filename = file_name.ptr,
                                                     .cur_errno = errno,
                                                 },
                                         }};
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

        return (struct rc_lexer){
            .result = OK_RESULT,
            .ok = lexer,
        };
}

static bool is_digit(char ch) { return '0' <= ch && ch <= '9'; }

static bool is_valid_identifier_char(char ch) {
        return ('a' <= ch && ch <= 'z') || ('A' <= ch && ch <= 'Z') ||
               is_digit(ch);
}

static struct token lookup_ident(struct string literal) {
        if (strcmp("bin", literal.ptr) == 0) {
                return (struct token){
                        .type = token_type_bin,
                        .literal = literal,
                };
        } else if (strcmp("lib", literal.ptr) == 0) {
                return (struct token){
                        .type = token_type_lib,
                        .literal = literal,
                };
        }
        else {
                return (struct token){
                        .type = token_type_ident,
                        .literal = literal,
                };
        }
}

static struct rc_token read_identifier(struct lexer *lexer) {
        GET_OR_RETURN(var_string_create(), str, var_string, rc_token);

        TRY_RC(var_string_append_c(&str, lexer->ch), rc_token);

        while (next_char(lexer), is_valid_identifier_char(lexer->ch)) {
                TRY_RC(var_string_append_c(&str, lexer->ch), rc_token);
        }

        GET_OR_RETURN(var_string_to_string(&str), string, string, rc_token);
        var_string_destroy(&str);

        return (struct rc_token){
            .ok = lookup_ident(string),
            .result = OK_RESULT,
        };
}

struct read_string_state {
        bool escaped;
};

static bool read_string_is_escaped(char ch, struct read_string_state *state) {
        if (ch == '\\') {
                state->escaped = true;
                return true;
        } else if (ch == '"' && state->escaped) {
                state->escaped = false;
                return true;
        } else {
                state->escaped = false;
        }

        return ch != '"' && ch != '\n' && ch != 0;
}

static struct rc_token read_string(struct lexer *lexer) {
        GET_OR_RETURN(var_string_create(), str, var_string, rc_token);
        TRY_RC(var_string_append_c(&str, lexer->ch), rc_token);

        struct read_string_state state = {false};

        while (next_char(lexer), read_string_is_escaped(lexer->ch, &state)) {
                TRY_RC(var_string_append_c(&str, lexer->ch), rc_token);
        }

        // Append "
        TRY_RC(var_string_append_c(&str, lexer->ch), rc_token);
        next_char(lexer);
        GET_OR_RETURN(var_string_to_string(&str), string, string, rc_token);
        var_string_destroy(&str);

        return (struct rc_token){
            .ok = {.type = token_type_string, .literal = string},
            .result = OK_RESULT,
        };
}

static void skip_whitespace(struct lexer *lexer) {
        while (lexer->ch == '\n' || lexer->ch == '\t' || lexer->ch == '\r' ||
               lexer->ch == ' ' || lexer->ch == 0) {
                next_char(lexer);
        }
}

struct rc_token next_token(struct lexer *lexer) {
        struct token tok = {0};
        skip_whitespace(lexer);

        switch (lexer->ch) {
        case EOF:
                tok.type = token_type_eof;
                tok.literal = string_malloc_empty();
                return (struct rc_token){
                    .result = OK_RESULT,
                    .ok = tok,
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
                return read_string(lexer);
        default:
                if (is_valid_identifier_char(lexer->ch)) {
                        return read_identifier(lexer);
                }
                tok.type = token_type_invalid;
                tok.literal = string_one_char(lexer->ch);
                return (struct rc_token){
                    .result = {.type = result_lexer_invalid_character,
                               .ctx =
                                   {
                                       .cur_errno = -1,
                                       .invalid_ch = lexer->ch,
                                   }},
                    .ok = tok,
                };
        }

        next_char(lexer);

        return (struct rc_token){.result = OK_RESULT, .ok = tok};
}

void destroy_lexer(struct lexer *lexer) { fclose(lexer->file); }
