#define TOKEN_IMPLEMENTATION

#include "./include/token.h"

Token token_init(Token_Type type, Token_Value value) {
    return (Token) {
        .type = type,
        .value = value,
    };
}

char *token_type_to_string(Token_Type type) {
    switch(type) {
        case TT_COLON:
            return "colon";
        case TT_COMMA:
            return "comma";
        case TT_LEFTB: 
            return "left brace";
        case TT_RIGHTB:
            return "right brace";
        case TT_STRING:
            return "string";
        case TT_NUMBER:
            return "number";
        case TT_FIELD:
            return "field";
        case TT_POINT:
            return "point";
        case TT_ROOT:
            return "root";
        default:
            fprintf(stderr, "ERROR: type not found\n");
            exit(EXIT_FAILURE);
    }
}

void token_to_string(Token token) {
    char *content = view_to_string(token.value);
    printf("value: %s, type: %s\n", content, token_type_to_string(token.type));
    free(content);
}