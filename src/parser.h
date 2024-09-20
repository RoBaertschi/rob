#ifndef rob_parser_h
#define rob_parser_h

#include "common.h"
#include "lexer.h"

struct parser {
        struct lexer lexer;

        struct token cur_token;
        struct token peek_token;

        char **errors;
};

result_container(parser, struct parser);

struct rc_parser create_parser(struct string file_name);

enum target_type {
        target_type_binary,
        target_type_library,
};

struct target_statement {
        enum target_type type;
        struct string ident;
        struct string *files;
};

enum statement_type {
        stmt_type_target, // bin main - "src/main.c"
};

struct statement {
        enum statement_type type;
        union {
                struct target_statement target;
        };
};

struct program {
        struct statement *statements;
        size_t len;
};

result_container(statement, struct statement);
result_container(target_statement, struct target_statement);

struct rc_statement parse_statement(struct parser *parser);

#endif // rob_parser_h
