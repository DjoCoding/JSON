#ifndef JSON_PARSER_H
#define JSON_PARSER_H

#include "jsonlexer.h"
#include "jsondefs.h"

struct _JSON_Parser {
    Token_List tokens;
    size_t current;
};
typedef struct _JSON_Parser JSON_Parser;

void JSON_Parser_init(JSON_Parser *parser, Token_List tokens);
JSON_Node *JSON_parse(char *content);

JSON_Array  *JSON_Parser_parse_array(JSON_Parser *parser);
JSON_Object *JSON_Parser_parse_json_object(JSON_Parser *parser);

#define JSON_PARSER_IMPLEMENTATION

void JSON_Parser_init(JSON_Parser *parser, Token_List tokens) {
    parser->tokens = tokens;
    parser->current = 0;
}

static Token JSON_Parser_peek(JSON_Parser *parser) {
    return (parser->tokens.items[parser->current]);
}

static void JSON_Parser_consume(JSON_Parser *parser) {
    parser->current++;
}

static int JSON_Parser_end(JSON_Parser *parser) {
    return (parser->current >= parser->tokens.count);
}

char *JSON_Parser_parse_string(JSON_Parser *parser) {
    if (JSON_Parser_end(parser)) EXIT_ERROR("Expected a <string> token but end found\n");

    Token token = JSON_Parser_peek(parser);
    if (token.type != TT_STRING) EXIT_ERROR("Invalid token type during parsing a string\n");

    char *string = view_to_string(&token.value);

    // consuming the value
    JSON_Parser_consume(parser);

    return string;
}

long JSON_Parser_parse_number(JSON_Parser *parser) {
    if (JSON_Parser_end(parser)) EXIT_ERROR("Expected a <number> token but end found\n");

    Token token = JSON_Parser_peek(parser);
    if (token.type != TT_NUMBER) EXIT_ERROR("Invalid token type during parsing a number\n");

    char *number = view_to_string(&token.value);
    long value = atoll(number);
    free(number);

    // consuming the value
    JSON_Parser_consume(parser);

    return value;
} 

JSON_Node *JSON_Parser_parse_json_node(JSON_Parser *parser) {
    if (JSON_Parser_end(parser))  EXIT_ERROR("Expected a <key> token but end found\n");

    Token token = JSON_Parser_peek(parser);
    if (token.type != TT_STRING) EXIT_ERROR("Invalid token type during parsing an object node, expected <key> token\n");

    JSON_Node *node = malloc(sizeof(JSON_Node));
    node->key = view_to_string(&token.value);

    // consuming the key token 
    JSON_Parser_consume(parser);

    if (JSON_Parser_end(parser))  EXIT_ERROR("Expected ':' token but end found\n");

    token = JSON_Parser_peek(parser);
    if (token.type != TT_COLON) EXIT_ERROR("Invalid token type during parsing an object node, expected ':' token")


    // consuming the ':' token
    JSON_Parser_consume(parser);

    token = JSON_Parser_peek(parser);

    switch (token.type) {
        case TT_STRING: 
            node->type = JSON_NODE_TYPE_STRING;
            node->value.as_string = JSON_Parser_parse_string(parser);
            break;
        case TT_NUMBER:
            node->type = JSON_NODE_TYPE_NUMBER;
            node->value.as_number = JSON_Parser_parse_number(parser);
            break;
        case TT_LEFT_BRACE:
            node->type = JSON_NODE_TYPE_JSON_ARRAY;
            node->value.as_json_array = JSON_Parser_parse_array(parser);
            break;
        case TT_LEFT_CURLY_BRACE:
            node->type = JSON_NODE_TYPE_JSON_OBJECT;
            node->value.as_json_object = JSON_Parser_parse_json_object(parser);
            break;
        default:
            EXIT_ERROR("Invalid token type during the parsing of the value of the json node\n");
    }

    return node;
}

JSON_Object *JSON_Parser_parse_json_object(JSON_Parser *parser) {
    if (JSON_Parser_end(parser))  EXIT_ERROR("Expected '{' token but end found\n");

    Token token = JSON_Parser_peek(parser);
    if (token.type != TT_LEFT_CURLY_BRACE) EXIT_ERROR("Invalid token type during parsing an object, expected '{' token\n");
    
    // consuming the '{' token
    JSON_Parser_consume(parser);

    if (JSON_Parser_end(parser)) EXIT_ERROR("Expected more tokens but end found\n");

    JSON_Object *object = malloc(sizeof(JSON_Object));
    DA_INIT(object, sizeof(JSON_Node));

    token = JSON_Parser_peek(parser);

    // handle the empty object case
    if (token.type == TT_RIGHT_CURLY_BRACE) { JSON_Parser_consume(parser); return object; };

    while (!JSON_Parser_end(parser)) {
        JSON_Node *node = JSON_Parser_parse_json_node(parser);
        DA_APPEND(object, node);

        if (JSON_Parser_end(parser)) EXIT_ERROR("Expected more tokens but end found\n");

        token = JSON_Parser_peek(parser);

        if (token.type == TT_RIGHT_CURLY_BRACE) { break; }
        if (token.type == TT_COMMA) { JSON_Parser_consume(parser); }
        else EXIT_ERROR("Invalid token found when parsing the json object, expected ',' token\n");
    }  

    // end of tokens without consuming the '}' token
    if (JSON_Parser_end(parser)) EXIT_ERROR("Expected more tokens but end found\n");

    token = JSON_Parser_peek(parser);
    if (token.type != TT_RIGHT_CURLY_BRACE) EXIT_ERROR("Invalid token type during parsing an array of object, expected '}' token\n");

    // consuming the '}' token
    JSON_Parser_consume(parser);

    return object;
}

JSON_Array *JSON_Parser_parse_array(JSON_Parser *parser) {
    if (JSON_Parser_end(parser))  EXIT_ERROR("Expected '[' token but end found\n");

    Token token = JSON_Parser_peek(parser);
    if (token.type != TT_LEFT_BRACE) EXIT_ERROR("Invalid token type during parsing an array of object, expected '[' token\n");
    
    // consuming the '[' token
    JSON_Parser_consume(parser);

    if (JSON_Parser_end(parser)) EXIT_ERROR("Expected more tokens but end found\n");

    JSON_Array *array = malloc(sizeof(JSON_Array));
    DA_INIT(array, sizeof(JSON_Object));

    token = JSON_Parser_peek(parser);

    // handle the empty array case
    if (token.type == TT_RIGHT_BRACE) { JSON_Parser_consume(parser); return array; }

    while (!JSON_Parser_end(parser)) {
        JSON_Object *object = JSON_Parser_parse_json_object(parser);
        DA_APPEND(array, object);

        if (JSON_Parser_end(parser)) EXIT_ERROR("Expected more tokens but end found\n");

        token = JSON_Parser_peek(parser);
        if (token.type == TT_RIGHT_BRACE) { break; }
        if (token.type == TT_COMMA) { JSON_Parser_consume(parser); }
        else EXIT_ERROR("Invalid token found when parsing the json array, expected ',' token\n");
    }

    // end of tokens without consuming the ']' token
    if (JSON_Parser_end(parser)) EXIT_ERROR("Expected more tokens but end found\n");

    token = JSON_Parser_peek(parser);
    if (token.type != TT_RIGHT_BRACE) EXIT_ERROR("Invalid token type during parsing an array of object, expected ']' token\n");

    // consuming the ']' token
    JSON_Parser_consume(parser);

    return array;
}

// parsing the root object
JSON_Node *JSON_Parser_parse(JSON_Parser *parser) {
    JSON_Node *root = malloc(sizeof(JSON_Node));

    Token token = JSON_Parser_peek(parser);

    root->key = NULL;

    switch(token.type) {
        case TT_LEFT_BRACE:
            root->type = JSON_NODE_TYPE_JSON_ARRAY; 
            root->value.as_json_array =  JSON_Parser_parse_array(parser);
            break;
        case TT_LEFT_CURLY_BRACE:
            root->type = JSON_NODE_TYPE_JSON_OBJECT;
            root->value.as_json_object =  JSON_Parser_parse_json_object(parser);
            break;
        default:    
            EXIT_ERROR("Invalid token type found during the parsing\n");
    }

    return root;
}

JSON_Node *JSON_parse(char *content) {
    if (content == NULL) return NULL;

    Token_List tokens = JSON_lex(content);
    
    JSON_Parser parser;
    JSON_Parser_init(&parser, tokens);
    JSON_Node *root = JSON_Parser_parse(&parser);

    return root;
}

#endif // JSON_PARSER_H