#ifndef TOKEN_H
#define TOKEN_H

#include "./view.h"

typedef enum {
    // json file to json tokens token types
    TT_STRING,
    TT_COLON,
    TT_LEFTB,
    TT_RIGHTB,
    TT_COMMA,
    TT_NUMBER,

    // modifier token types
    TT_ROOT,
    TT_FIELD,
    TT_POINT,
} Token_Type;

typedef View Token_Value;

typedef struct {
    Token_Type type;
    Token_Value value;
} Token;

typedef struct {
    Token *items;
    size_t count;
    size_t size;
} Tokens;

Token token_init(Token_Type type, Token_Value value);
void token_to_string(Token token);

#endif