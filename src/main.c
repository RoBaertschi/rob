#include "common.h"
#include "lexer.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

int main(void) {

        struct rc_lexer rc_lexer = create_lexer(string_from_array("Robfile"));

        if (rc_lexer.result.type != result_ok) {
                printf("failed to create lexer: %s", rc_error_str(rc_lexer));
        }

        struct lexer lexer = rc_lexer.ok;

        while (true) {
                struct rc_token rc_tok = next_token(&lexer);
                if (rc_tok.result.type != result_ok) {
                        printf("failed to get next token: %s",
                               rc_error_str(rc_tok));
                        break;
                } else {
                        printf("tok: %s literal: %s\n",
                               token_type_str(rc_tok.ok.type),
                               rc_tok.ok.literal.ptr);
                }
                if (rc_tok.ok.type == token_type_eof) {
                        break;
                }
        }

        return EXIT_SUCCESS;
}
