#include "token.h"

char *token_type_str(enum token_type token) {
        switch (token) {
        case token_type_eof:
                return "EOF";
        case token_type_ident:
                return "identifier";
        case token_type_comma:
                return "comma";
        case token_type_equal:
                return "equal";
        case token_type_minus:
                return "minus";
        case token_type_semicolon:
                return "semicolon";
        case token_type_string:
                return "string";
        case token_type_invalid:
                return "invalid char";
        case token_type_bin:
                return "bin";
        case token_type_lib:
                return "lib";
        default:
                return "invalid token";
        }
}
