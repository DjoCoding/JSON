#ifndef PARSER_H
#define PARSER_H

#include "./token.h"
#include "./da.h"

typedef enum {
    PARSER_NO_ERROR,
    PARSER_UNEXPECTED_TOKEN,
} Parser_Error;

typedef struct {
    Tokens tokens;
    size_t current;
    Parser_Error error;
} Parser;

typedef struct JSON JSON;

typedef enum {
    NODE_TYPE_NUMBER,
    NODE_TYPE_STRING,
    NODE_TYPE_JSON,
} Node_Type;

typedef union {
    long as_int;
    char *as_str;
    JSON *as_json;
} Node_Value;

typedef struct  {
    char *key;
    Node_Type type;
    Node_Value value;
} JSON_Node;

struct JSON {
    JSON_Node **items;
    size_t count;
    size_t size;
};


Parser parser_init(Tokens tokens);
JSON *parse(Parser *parser);
void json_to_string(JSON *json);

#endif