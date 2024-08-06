#ifndef JSON_PARSER_H
#define JSON_PARSER_H

#include "json-defs.h"
#include "json-clean.h"
#define JSON_PRINTER_IMPLEMENTATION
#include "json-pretty-printer.h"

// Parse the json string
bool json_parse(JSON_Parser *self);

#ifdef JSON_PARSER_IMPLEMENTATION

#define ppeek     json_parser_peek
#define padvance  json_parser_advance
#define peot      json_parser_eot
#define pinteger  json_parser_parse_integer
#define pfloat    json_parser_parse_float
#define pexpect   json_parser_expect
#define pobject   json_parser_parse_object
#define pitem     json_parser_parse_item
#define parse     json_parse

JSON json_parser_parse_object(JSON_Parser *self);

static JSON_Token json_parser_peek(JSON_Parser *self) {
    return (self->tokens.items[self->curr_token]);
}

static void json_parser_advance(JSON_Parser *self) {
    self->curr_token++;
}

static bool json_parser_eot(JSON_Parser *self) {
    return (self->curr_token >= self->tokens.count);
}

static bool json_parser_expect(JSON_Parser *self, JSON_Token_Kind kind, JSON_Token *token) {
    JSON_Token current = ppeek(self);
    if (token) *token = current;
    if (current.kind != kind) { return false; }
    padvance(self);
    return true;
} 

static int64_t json_parser_parse_integer(JSON_Parser *self) {
    JSON_Token token = ppeek(self);
    int64_t result = 0;
    if (!sv_parse_integer(token.text, &result)) {
        JSON_THROW_ERROR(self, "%s:" LOC_FMT ": failed to parse the integer number " SV_FMT "\n", self->source.filename, LOC_UNWRAP(token.loc), SV_UNWRAP(token.text));
    }
    return result;
}

static double json_parser_parse_float(JSON_Parser *self) {
    JSON_Token token = ppeek(self);
    double result = 0;
    if (!sv_parse_float(token.text, &result)) {
        JSON_THROW_ERROR(self, "%s:" LOC_FMT ": failed to parse the floating point number " SV_FMT "\n", self->source.filename, LOC_UNWRAP(token.loc), SV_UNWRAP(token.text));
    }
    return result;   
}

JSON_Item json_parser_parse_item(JSON_Parser *self) {
    JSON_Item item = {0};

    if (peot(self)) { JSON_THROW_ERROR(self, "%s: expected token of type string but end of tokens found\n", self->source.filename);  goto return_defer; }

    JSON_Token token = {0};
    if (!pexpect(self, TOKEN_KIND_STRING, &token)) {
        JSON_THROW_ERROR(self, "%s:" LOC_FMT ": expected token of type string but `%s` found\n", self->source.filename, LOC_UNWRAP(token.loc), json_token_kind_to_cstr(token.kind));
        goto return_defer;
    } 

    item.key = token.text;

    if (peot(self)) {
        JSON_THROW_ERROR(self, "%s: expected `:` token but end of tokens found\n", self->source.filename);
        goto return_defer;
    }

    if (!pexpect(self, TOKEN_KIND_COLON, &token)) {
        JSON_THROW_ERROR(self, "%s:" LOC_FMT ": expected `:` token but `%s` found\n",self->source.filename, LOC_UNWRAP(token.loc), json_token_kind_to_cstr(token.kind));
        goto return_defer;
    }

    if (peot(self)) {
        JSON_THROW_ERROR(self, "%s: expected token of type `value` but end of tokens found\n", self->source.filename);
        goto return_defer;
    }

    token = ppeek(self);

    if (token.kind == TOKEN_KIND_STRING) {
        item.kind = JSON_NODE_KIND_STRING;
        item.as._string = token.text;
        goto advance_and_return;
    }

    if (token.kind == TOKEN_KIND_INTEGER) {
        item.kind = JSON_NODE_KIND_INTEGER;
        item.as._integer = pinteger(self);
        if (perror(self)) { goto return_defer; }
        goto advance_and_return;
    }

    if (token.kind == TOKEN_KIND_FLOAT) {
        item.kind = JSON_NODE_KIND_FLOAT;
        item.as._float = pfloat(self);
        if (perror(self)) { goto return_defer; }
        goto advance_and_return;
    }

    if (token.kind == TOKEN_KIND_LEFT_CURLY_BRACE){
        item.kind = JSON_NODE_KIND_OBJECT;
        item.as._object = pobject(self);
        return item;
    }

    JSON_THROW_ERROR(self, "%s" LOC_FMT ": unexpected token found %s\n", self->source.filename, LOC_UNWRAP(token.loc), json_token_kind_to_cstr(token.kind));
    goto return_defer;


advance_and_return:
    padvance(self);
    return item;

return_defer:
    item = (JSON_Item) {0};
    return item;
}

JSON json_parser_parse_object(JSON_Parser *self) {
    JSON result = malloc(sizeof(*result));

    if (peot(self)) { JSON_THROW_ERROR(self, "%s: expected `{` token but end of tokens found\n", self->source.filename);  goto return_defer; }
    
    JSON_Token token = {0};
    if (!pexpect(self, TOKEN_KIND_LEFT_CURLY_BRACE, &token)) {
        JSON_THROW_ERROR(self, "%s" LOC_FMT ": expected `{` token but `%s` found\n", self->source.filename, LOC_UNWRAP(token.loc), json_token_kind_to_cstr(token.kind));
        goto return_defer;
    } 

    if (peot(self)) {
        JSON_THROW_ERROR(self, "%s: expected  `}` token but end of tokens found\n", self->source.filename);
        goto return_defer;
    }

    DA_INIT(result, sizeof(JSON_Item));

    // support for empty objects
    token = ppeek(self);
    if (token.kind == TOKEN_KIND_RIGHT_CURLY_BRACE) {
        padvance(self);
        return result;
    }
    

    while (!peot(self)) {
        JSON_Item item = json_parser_parse_item(self);
        if (perror(self)) { goto return_defer; }

        if (peot(self)) {
            JSON_THROW_ERROR(self, "%s: expected `}` token but end of tokens found\n", self->source.filename);
            goto return_defer;
        }

        DA_APPEND(result, item);

        token = ppeek(self);
        if (token.kind == TOKEN_KIND_RIGHT_CURLY_BRACE) {
            padvance(self);
            return result;
        }

        if (!pexpect(self, TOKEN_KIND_COMMA, &token)) {
            JSON_THROW_ERROR(self, "%s" LOC_FMT ": expected `}` token but %s found\n", self->source.filename, LOC_UNWRAP(token.loc), json_token_kind_to_cstr(token.kind));
            goto return_defer;
        }
    }

return_defer:
    json_clean_object(result);
    free(result);
    return NULL;
}

bool json_parse(JSON_Parser *self) {
    if (peot(self)) { return (JSON) {0}; }
    self->root = json_parser_parse_object(self);
    return !perror(self);
}

#endif

#endif // JSON_PARSER_H