#ifndef GETTER_H
#define GETTER_H

#include "./include/view.h"
#include "./include/lib.h"

typedef enum {
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


#endif