#ifndef JSON_LEXER_H
#define JSON_LEXER_H

#include "json-defs.h"
#include "json-error.h"
#include "json-clean.h"
#include "../tools/dyn-arr.h"
#include <ctype.h>

// Lex the json string
void json_lexer_lex(JSON_Parser *self);

#ifdef JSON_LEXER_IMPLEMENTATION

#define lpeek    json_lexer_peek
#define ladvance json_lexer_advance
#define leof     json_lexer_eof
#define lread    json_lexer_read_token
#define lex      json_lexer_lex

static char json_lexer_peek(JSON_Parser *self) {
    return (self->source.text.content[self->lexer.current]);
}

static void json_lexer_advance(JSON_Parser *self) {
    char current = lpeek(self);
    self->lexer.current++;
    self->lexer.loc.col++;
    if (current == '\n') { self->lexer.loc.row++; self->lexer.loc.col = 1; }
}

static bool json_lexer_eof(JSON_Parser *self) {
    return (self->source.text.count <= self->lexer.current);
}

static bool json_lexer_lex_number(JSON_Parser *self, JSON_Token *token) {
    char current = lpeek(self);
    if (!isdigit(current)) { return false; }

    char *begin = &self->source.text.content[self->lexer.current];
    size_t size = 0;
    bool is_float = false;

    while(!leof(self)) {
        current = lpeek(self);
        if (current != '.' && !isdigit(current)) { break; }
        if (current == '.') { is_float = true; }
        size++;
        ladvance(self);
    }
    
    token->text = SV_GET(begin, size);

    if (is_float) { token->kind = TOKEN_KIND_FLOAT; }
    else { token->kind = TOKEN_KIND_INTEGER; }

    return true;
}

static JSON_Token json_lexer_read_token(JSON_Parser *self) {
    JSON_Token token = {0};
    token.loc = self->lexer.loc;

    char current = lpeek(self);

    if (current == '{') {
        ladvance(self);
        token.kind = TOKEN_KIND_LEFT_CURLY_BRACE;
        token.text = SV(NULL);
        return token;
    }

    if (current == '}') {
        ladvance(self);
        token.kind = TOKEN_KIND_RIGHT_CURLY_BRACE;
        token.text = SV(NULL);
        return token;
    }

    if (current == ':') {
        ladvance(self);
        token.kind = TOKEN_KIND_COLON;
        token.text = SV(NULL);
        return token;
    }

    if (current == ',') {
        ladvance(self);
        token.kind = TOKEN_KIND_COMMA;
        token.text = SV(NULL);
        return token;
    }

    char *begin = &self->source.text.content[self->lexer.current];
    size_t size = 0;

    if (current == '"') {
        begin++;
        ladvance(self); 

        current = 0; // added for checks later

        while (!leof(self)) {
            current = lpeek(self);
            ladvance(self);
            if (current == '"') { break; }
            size++;
        }

        if (current != '"') {
            String_View original_string = SV_GET(begin, size);
            String_View unescaped_string = unescape_string_to_sv(original_string);
            JSON_THROW_ERROR(self, "%s:" LOC_FMT ": failed to parse the string \"" SV_FMT "\"\n", self->source.filename, LOC_UNWRAP(token.loc), SV_UNWRAP(unescaped_string));
            free(unescaped_string.content);
            return token;
        }

        token.kind = TOKEN_KIND_STRING;
        token.text = SV_GET(begin, size);

        return token;
    }

    if (current == '-') {
        ladvance(self);
        if (!json_lexer_lex_number(self, &token)) {
            JSON_THROW_ERROR(self, "%s:" LOC_FMT ": failed to parse the number \n", self->source.filename, LOC_UNWRAP(token.loc));
        }
        token.text.content--;
        token.text.count++;
        return token;
    }

    if (isdigit(current)) {
        json_lexer_lex_number(self, &token);
        return token;
    }

    JSON_THROW_ERROR(self, "%s" LOC_FMT ": failed to identify the char `%c`\n", self->source.filename, LOC_UNWRAP(token.loc), current);
    return token;
}

void json_lexer_lex(JSON_Parser *self) {
    DA_INIT(&self->tokens, sizeof(JSON_Token));
    while (!leof(self)) {
        char current = lpeek(self);
        if (isspace(current)) { ladvance(self); }
        else {
            JSON_Token token = lread(self);
            if (perror(self)) { goto return_defer; }
            DA_APPEND(&self->tokens, token);
        }
    }

    return;

return_defer:
    free(self->tokens.items);
    self->tokens.items = NULL;
    return;
}

#endif


#endif // JSON_LEXER_H