#ifndef JSON_LEXER_H
#define JSON_LEXER_H

#include <ctype.h>
#include "view.h"
#include "jsontokens.h"
#include "utils.h"

//TODO: the JSON_Lexer 'get_next' method is taking the token value as a string view, gotta convert that to a heap allocated
//      string of the same value, so we can free the file content later

struct _JSON_Lexer {
    View input;
    size_t current;
};
typedef struct _JSON_Lexer JSON_Lexer;

void JSON_Lexer_init(JSON_Lexer *lexer, char *file_content);
Token_List JSON_lex(char *content);

#define JSON_LEXER_IMPLEMENTATION

void JSON_Lexer_init(JSON_Lexer *lexer, char *file_content) {
    lexer->input = view_init(file_content);
    lexer->current = 0;
}

static char JSON_Lexer_peek(JSON_Lexer *lexer) {
    return (lexer->input.content[lexer->current]);
}

static void JSON_Lexer_consume(JSON_Lexer *lexer) {
    lexer->current++;
}

static int JSON_Lexer_end(JSON_Lexer *lexer) {
    return (lexer->input.size <= lexer->current);
}

static Token JSON_Lexer_get_next(JSON_Lexer *lexer) {
    char ch = JSON_Lexer_peek(lexer);

    if (ch == '[') {
        JSON_Lexer_consume(lexer);
        return (Token) { .type = TT_LEFT_BRACE, .value = view_init(NULL) };
    }

    if (ch == ']') {
        JSON_Lexer_consume(lexer);
        return (Token) { .type = TT_RIGHT_BRACE, .value = view_init(NULL) };
    }

    if (ch == '{') {
        JSON_Lexer_consume(lexer);
        return (Token) { .type = TT_LEFT_CURLY_BRACE, .value = view_init(NULL) };
    }

    if (ch == '}') {
        JSON_Lexer_consume(lexer);
        return (Token) { .type = TT_RIGHT_CURLY_BRACE, .value = view_init(NULL) };
    }

    if (ch == ',') {
        JSON_Lexer_consume(lexer);
        return (Token) { .type = TT_COMMA, .value = view_init(NULL) };
    }

    if (ch == ':') {
        JSON_Lexer_consume(lexer);
        return (Token) { .type = TT_COLON, .value = view_init(NULL) };
    }

    char *begin = lexer->current + lexer->input.content;
    size_t size = 0;

    // lexing a string
    if (ch == '"') {
        JSON_Lexer_consume(lexer);
        begin++;
        while (!JSON_Lexer_end(lexer)) {
            ch = JSON_Lexer_peek(lexer);
            JSON_Lexer_consume(lexer);
            if (ch == '"') { break; }
            size++;
        }

        View view = view_init(begin);
        view.size = size;

        return (Token) { .type = TT_STRING, .value = view };
    }

    if (isdigit(ch)) {
        while (!JSON_Lexer_end(lexer)) {
            if (!isdigit(JSON_Lexer_peek(lexer))) { break; }
            JSON_Lexer_consume(lexer);
            size++;
        }

        View view = view_init(begin);
        view.size = size;

        return (Token) { .type = TT_NUMBER, .value = view };
    }

    EXIT_ERROR("Invalid token type: %c\n", ch);
}

Token_List JSON_Lexer_lex(JSON_Lexer *lexer) {
    Token_List tokens;
    DA_INIT(&tokens, sizeof(Token));

    while (!JSON_Lexer_end(lexer)) {
        if (isspace(JSON_Lexer_peek(lexer))) {
            JSON_Lexer_consume(lexer);
        } else {
            Token token = JSON_Lexer_get_next(lexer);
            DA_APPEND(&tokens, token);
        }
    }

    return tokens;
}

Token_List JSON_lex(char *content) {
    JSON_Lexer lexer;
    JSON_Lexer_init(&lexer, content);
    return JSON_Lexer_lex(&lexer);
}

char *get_token_type(Token token) {
    switch(token.type) {
        case TT_STRING: return "string";
        case TT_NUMBER: return "number";
        case TT_COLON: return ":";
        case TT_COMMA: return ",";
        case TT_LEFT_BRACE: return "[";
        case TT_RIGHT_BRACE: return "]";
        case TT_LEFT_CURLY_BRACE: return "{";
        case TT_RIGHT_CURLY_BRACE: return "}";
        default: 
            RETURN_ERROR(NULL, "Couldn't find the token type\n");
    }
}

void print_tokens(Token_List tokens) {
    for (size_t i = 0; i < tokens.count; ++i) {
        printf("%s\n", get_token_type(tokens.items[i]));
    }
}


#endif // JSON_LEXER_H