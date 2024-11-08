#ifndef JSON_TOKENS_H
#define JSON_TOKENS_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "../utils/sv.h"

#define JSON_TOKEN_ARRAY_INITIAL_CAPACITY 10

typedef enum {
    JSON_TOKEN_NONE = 0,
    
    JSON_TOKEN_SINGLE_SYMBOL_BEGIN,
        JSON_TOKEN_KIND_LEFT_BRACKET_BRACE,
        JSON_TOKEN_KIND_RIGHT_BRACKET_BRACE,
        JSON_TOKEN_KIND_LEFT_CURLY_BRACE,
        JSON_TOKEN_KIND_RIGHT_CURLY_BRACE,
        JSON_TOKEN_KIND_COMMA,
        JSON_TOKEN_KIND_DOUBLE_POINTS,
    JSON_TOKEN_SINGLE_SYMBOLS_END,
    
    JSON_TOKEN_KIND_STRING,
    JSON_TOKEN_KIND_INTEGER,
} JSON_Token_Kind;

#define JSON_TOKEN_SINGLE_SYMBOLS_COUNT JSON_TOKEN_SINGLE_SYMBOLS_END - JSON_TOKEN_SINGLE_SYMBOL_BEGIN - 1

typedef union {
    int64_t integer;
    String_View string;
} JSON_Token_As;

typedef struct {
    JSON_Token_Kind kind;
    JSON_Token_As   as;
    size_t          line;
} JSON_Token;   

typedef struct {
    JSON_Token *items;
    size_t count;
    size_t size;
} JSON_Tokens;

JSON_Token json_token_integer_init(uint64_t integer, size_t line) {
    return (JSON_Token) { .kind = JSON_TOKEN_KIND_INTEGER, .as.integer = integer, .line = line };
}

JSON_Token json_token_string_init(String_View s, size_t line) {
    return (JSON_Token) { .kind = JSON_TOKEN_KIND_STRING, .as.string = s, .line = line };
}

JSON_Token json_token_init(JSON_Token_Kind kind, size_t line) {
    return (JSON_Token) { .kind = kind, .line = line };
}

JSON_Tokens json_tokens() {
    JSON_Tokens tokens = {0};
    tokens.size = JSON_TOKEN_ARRAY_INITIAL_CAPACITY;
    tokens.items = (JSON_Token *) malloc(sizeof(JSON_Token) * JSON_TOKEN_ARRAY_INITIAL_CAPACITY);
    if(!tokens.items) { 
        exit(1);
    }
    return tokens;
}

void json_tokens_resize(JSON_Tokens *this) {
    this->size *= 2;
    this->items = (JSON_Token *) realloc(this->items, sizeof(JSON_Token) * this->size);
    if (!this->items) { 
        exit(1);
    }
}

int json_tokens_full(JSON_Tokens *this) {
    return this->count >= this->size;
}

void json_tokens_append(JSON_Tokens *this, JSON_Token token) {
    if (json_tokens_full(this)) { json_tokens_resize(this); }
    this->items[this->count++] = token;
}

void json_tokens_free(JSON_Tokens *this) {
    if (this->items) { free(this->items); }
}

#endif