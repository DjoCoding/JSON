#ifndef JSON_LEXER_H
#define JSON_LEXER_H

#include <assert.h>
#include <ctype.h>
#include "./jerror.h"
#include "./jtoken.h"

#define JSON_RETURN_LEXER_ERROR(lexer, error_fmt, ...) \
    do { \
        JSON_SET_ERROR((lexer)->error, error_fmt, __VA_ARGS__); \
        return json_token_init(JSON_TOKEN_NONE); \
    } while(0)

typedef struct {
    String_View s;
    size_t      current;
    JSON_Error  error;
    size_t      line;
} JSON_Lexer;   


JSON_Lexer json_lexer(char *content) {
    JSON_Lexer l = {0};
    l.s          = sv_from_cstr(content);
    l.line       = 1;
    return l;
}

char *json_lexer_peek_pointer(JSON_Lexer *this) {
    assert(this->current < this->s.count);
    return &this->s.content[this->current];
}

char json_lexer_peek(JSON_Lexer *this) {
    assert(this->current < this->s.count);
    return this->s.content[this->current];
}

void json_lexer_advance(JSON_Lexer *this) {
    if (json_lexer_peek(this) == '\n') { ++this->line; }
    ++this->current;
}

int json_lexer_end_of_tokens(JSON_Lexer *this) {
    return this->current >= this->s.count;
}

void json_lexer_skip(JSON_Lexer *this) {
    while(!json_lexer_end_of_tokens(this)) {
        if (!isspace(json_lexer_peek(this))) { break; }
        json_lexer_advance(this);
    }
}

JSON_Token json_lexer_lex_token(JSON_Lexer *this) {
    char c = json_lexer_peek(this);

    char symbols[JSON_TOKEN_SINGLE_SYMBOLS_COUNT] = {
        '[',
        ']',
        '{',
        '}',
        ',',
        ':'
    };

    for(size_t i = 0; i < JSON_TOKEN_SINGLE_SYMBOLS_COUNT; ++i) {
        if (c == symbols[i]) {
            json_lexer_advance(this);
            JSON_Token_Kind kind = i + JSON_TOKEN_SINGLE_SYMBOL_BEGIN + 1;
            return json_token_init(kind);
        }
    }

    char *begin = NULL;

    if(c == '"') {
        json_lexer_advance(this);
        begin = json_lexer_peek_pointer(this);
        size_t size = 0;

        while(!json_lexer_end_of_tokens(this)) {
            char current = json_lexer_peek(this);
            json_lexer_advance(this);
        
            if (current == '"') { 
                return json_token_string_init(sv(begin, size));
            }

            ++size;
        }
        JSON_RETURN_LEXER_ERROR(this,
                "Unexpected end of input at line: %zu, expected a quote to terminate the string " SV_FMT "\n", 
                this->line, 
                SV_UNWRAP(sv(begin, size))
                );
    }

    if (isdigit(c)) {
        begin = json_lexer_peek_pointer(this);
        size_t size = 0;

        while(!json_lexer_end_of_tokens(this)) {
            char current = json_lexer_peek(this);
            
            if (!isdigit(current)) { 
                return json_token_integer_init(int_from_sv(sv(begin, size)));
            } 

            json_lexer_advance(this);
            ++size;
        }

        return json_token_integer_init(int_from_sv(sv(begin, size)));
    }

    JSON_RETURN_LEXER_ERROR(this,
            "Unexpected character %c at line: %zu\n", 
            c, 
            this->line
        );
}

JSON_Tokens json_lex(JSON_Lexer *this) {
    JSON_Tokens tokens = json_tokens(); 

    while(!json_lexer_end_of_tokens(this)) {
        json_lexer_skip(this);
        JSON_Token token = json_lexer_lex_token(this);
        if (token.kind == JSON_TOKEN_NONE)  { break; }
        json_tokens_append(&tokens, token);
    }

    return tokens;
}





#endif