#ifndef PARSER_H
#define PARSER_H

#include "./token.h"
#include "./da.h"
#include "./json.h"

typedef enum {
    PARSER_NO_ERROR,
    PARSER_UNEXPECTED_TOKEN,
} Parser_Error;

typedef struct {
    Tokens tokens;
    size_t current;
    Parser_Error error;
} Parser;


Parser parser_init(Tokens tokens);
JSON *parse(Parser *parser);
int parser_expect(Parser *parser, Token_Type type);
JSON_Node *json_get_json_node(JSON *obj, Tokens tokens);

#endif