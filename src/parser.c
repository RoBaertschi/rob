#include "parser.h"
#include "common.h"
#include "lexer.h"
#include "stb_ds.h"
#include "token.h"

static struct result parser_next_token(struct parser *parser) {
        parser->cur_token = parser->peek_token;
        TRY_AND_GET(next_token(&parser->lexer), parser->peek_token, token);
        return OK_RESULT;
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-braces"

struct rc_parser create_parser(struct string file_name) {

        GET_OR_RETURN(create_lexer(file_name), lexer, lexer, rc_parser);

        struct parser parser = {
            .lexer = lexer,
            .errors = NULL,
            .cur_token = {0},
            .peek_token = {0},
        };

        TRY_RC(parser_next_token(&parser), rc_parser);
        TRY_RC(parser_next_token(&parser), rc_parser);

        return (struct rc_parser){
            .result = OK_RESULT,
            .ok = parser,
        };
}
#pragma GCC diagnostic pop

static struct result expect_peek(struct parser *parser, enum token_type type) {
        if (parser->peek_token.type == type) {
                return OK_RESULT;
        } else {
                return (struct result){
                        .type = result_parser_unexpected_token,
                        .ctx = {
                                .unexpected_token = {
                                        parser->peek_token.type, type
                                }
                        },
                };
        }
}

static struct rc_target_statement parse_target_stmt(struct parser *parser){
        TRY_RC(expect_peek(parser, token_type_ident), rc_target_statement);
        TRY_RC(parser_next_token(parser), rc_target_statement);
}

struct rc_statement parse_statement(struct parser *parser) {
        switch (parser->cur_token.type) {
        case token_type_lib:
        case token_type_bin:
                ;
                GET_OR_RETURN(parse_target_stmt(parser), stmt, target_statement, rc_statement);
                return (struct rc_statement){
                                .ok = {
                                        .type = stmt_type_target,
                                        .target = stmt,
                                },
                                .result = OK_RESULT,
                        };
        default:
                arrput(
                    parser->errors,
                    sprintf_alloc(
                        "could not make statement for token '%s' with literal",
                        token_type_str(parser->cur_token.type),
                        parser->cur_token.literal.ptr));

                return (struct rc_statement){
                    .ok = {0},
                    .result = {.type = result_parser_invalid_token,
                               .ctx =
                                   {
                                       .cur_errno = 0,
                                       .invalid_token = parser->cur_token,
                                   }},
                };
        }
}
