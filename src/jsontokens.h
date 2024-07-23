#ifndef JSON_TOKENS_H
#define JSON_TOKENS_H

#include "view.h"

enum _Token_Type {
    TT_STRING, 
    TT_NUMBER,
    TT_LEFT_CURLY_BRACE,
    TT_RIGHT_CURLY_BRACE,
    TT_LEFT_BRACE,
    TT_RIGHT_BRACE,
    TT_COMMA,
    TT_COLON
};
typedef enum _Token_Type Token_Type;

typedef View Token_Value;

struct _Token {
    Token_Type type;
    Token_Value value;
};
typedef struct _Token Token;

struct _Token_List {
    Token *items;
    size_t count;
    size_t size;
};
typedef struct _Token_List Token_List;


#endif // JSON_TOKENS_H