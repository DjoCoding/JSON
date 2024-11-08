#ifndef JSON_PARSER_H
#define JSON_PARSER_H

#include <assert.h>
#include "./jtoken.h"
#include "./jerror.h"
#include "./jnode.h"

typedef struct {
    JSON_Tokens tokens;
    size_t current;
    JSON_Error error;
} JSON_Parser;

#define JSON_RETURN_PARSER_ERROR(parser, return_value, error_fmt, ...) \
    do { \
        JSON_SET_ERROR((parser)->error, "[PARSER] " error_fmt, __VA_ARGS__); \
        return return_value; \
    } while(0)

JSON_Object json_parse_object(JSON_Parser *this);
JSON_Array json_parse_array(JSON_Parser *this);

JSON_Parser json_parser(JSON_Tokens tokens) {
    JSON_Parser parser = {0};
    parser.tokens = tokens;
    return parser; 
}

JSON_Token json_parser_peek(JSON_Parser *this) {
    return this->tokens.items[this->current];
}

void json_parser_advance(JSON_Parser *this) {
    ++this->current;
}

int json_parser_end_of_tokens(JSON_Parser *this) {
    return this->current >= this->tokens.count;
}

int json_parser_has_error(JSON_Parser *this) {
    return this->error.has_error;
}

JSON_Token json_parser_last_token(JSON_Parser *this) {
    return this->tokens.items[this->tokens.count - 1];
}

JSON_Pair json_parse_pair(JSON_Parser *this) {
    JSON_Pair pair = {0};

    if (json_parser_end_of_tokens(this)) {
        JSON_RETURN_PARSER_ERROR(this, pair, "Unexpected end of input at line: %zu, expected a key-value pair to be parsed", json_parser_last_token(this).line);
    }

    JSON_Token key_token = json_parser_peek(this);

    if (key_token.kind != JSON_TOKEN_KIND_STRING) {
        JSON_RETURN_PARSER_ERROR(this, pair, "Expected key to be a string at line: %zu", key_token.line);
    }

    // consume the key token
    json_parser_advance(this);

    if (json_parser_end_of_tokens(this)) {
        JSON_RETURN_PARSER_ERROR(this, pair, "Expected a value associated to the key `" SV_FMT "` at line: %zu, but end of input found", SV_UNWRAP(key_token.as.string), json_parser_last_token(this).line);
    }

    if (json_parser_peek(this).kind != JSON_TOKEN_KIND_DOUBLE_POINTS) {
        JSON_RETURN_PARSER_ERROR(this, pair, "Expected `:` token after the key `" SV_FMT "` at line: %zu", SV_UNWRAP(key_token.as.string), json_parser_last_token(this).line);
    }

    // consume the ':' token
    json_parser_advance(this);

    if (json_parser_end_of_tokens(this)) {
        JSON_RETURN_PARSER_ERROR(this, pair, "Expected a value associated to the key `" SV_FMT "` at line: %zu, but end of input found", SV_UNWRAP(key_token.as.string), json_parser_last_token(this).line);
    }

    JSON_Token value_token = json_parser_peek(this);
    JSON_Pair_Value value = {0};

    switch(value_token.kind) {
        case JSON_TOKEN_KIND_STRING:
            value = json_pair_string_value(cstr_from_sv(value_token.as.string));
            json_parser_advance(this);
            break;
        case JSON_TOKEN_KIND_INTEGER:
            value = json_pair_integer_value(value_token.as.integer);
            json_parser_advance(this);
            break;
        case JSON_TOKEN_KIND_LEFT_CURLY_BRACE:
            JSON_Object object = json_parse_object(this);
            value = json_pair_object_value(object);
            break;
        case JSON_TOKEN_KIND_LEFT_BRACKET_BRACE:
            JSON_Array array = json_parse_array(this);
            value = json_pair_array_value(array);
            break;
        default:
            JSON_RETURN_PARSER_ERROR(this, pair, "Invalid token as the value associated to the key `" SV_FMT "` at line: %zu", SV_UNWRAP(key_token.as.string), json_parser_last_token(this).line);
    }

    return json_pair(cstr_from_sv(key_token.as.string), value);
}

JSON_Object json_parse_object(JSON_Parser *this) {
    JSON_Object object = json_object();

    if (json_parser_end_of_tokens(this)) {
        JSON_RETURN_PARSER_ERROR(this, object, "Unexpected end of input found at line: %zu, expected a json object to be parsed", json_parser_last_token(this).line);
    }

    if (json_parser_peek(this).kind != JSON_TOKEN_KIND_LEFT_CURLY_BRACE) {
        JSON_RETURN_PARSER_ERROR(this, object, "Unexpected start of json object found at line: %zu, expected '{' token", json_parser_peek(this).line);
    }

    json_parser_advance(this);

    if (json_parser_end_of_tokens(this)) {
        JSON_RETURN_PARSER_ERROR(this, object, "Unexpected end of input at line: %zu", json_parser_last_token(this).line);
    }

    if (json_parser_peek(this).kind == JSON_TOKEN_KIND_RIGHT_CURLY_BRACE) { 
        json_parser_advance(this);
        return object;
    }

    while(!json_parser_end_of_tokens(this)) {
        JSON_Pair pair = json_parse_pair(this);
        if (json_parser_has_error(this)) {
            return object;
        }

        json_object_append_pair(&object, pair);

        JSON_Token token = json_parser_peek(this);
        if (token.kind == JSON_TOKEN_KIND_COMMA) {
            json_parser_advance(this);
        } else {
            if (json_parser_peek(this).kind != JSON_TOKEN_KIND_RIGHT_CURLY_BRACE) { 
                JSON_RETURN_PARSER_ERROR(this, object, "Expected ',' or '}' at line: %zu, but none of them found", json_parser_peek(this).line);
            }

            json_parser_advance(this); break;
        }
    }

    return object; 
}

JSON_Array json_parse_array(JSON_Parser *this) {
    JSON_Array array = json_array();

    if (json_parser_end_of_tokens(this)) {
        JSON_RETURN_PARSER_ERROR(this, array, "Unexpected end of input found at line: %zu, expected a json array to be parsed", json_parser_last_token(this).line);
    }

    if (json_parser_peek(this).kind != JSON_TOKEN_KIND_LEFT_BRACKET_BRACE) {
        JSON_RETURN_PARSER_ERROR(this, array, "Unexpected start of json array found at line: %zu, expected '[' token", json_parser_peek(this).line);
    }

    json_parser_advance(this);

    if (json_parser_end_of_tokens(this)) {
        JSON_RETURN_PARSER_ERROR(this, array, "Unexpected end of input at line: %zu", json_parser_last_token(this).line);
    }

    if (json_parser_peek(this).kind == JSON_TOKEN_KIND_RIGHT_BRACKET_BRACE) { 
        json_parser_advance(this);
        return array;
    }

    while(!json_parser_end_of_tokens(this)) {
        JSON_Object object = json_parse_object(this);
        if(json_parser_has_error(this)) { 
            return array;
        }

        json_array_append_object(&array, object);

        JSON_Token token = json_parser_peek(this);
        if (token.kind == JSON_TOKEN_KIND_COMMA) {
            json_parser_advance(this);
        } else {
            if (json_parser_peek(this).kind != JSON_TOKEN_KIND_RIGHT_BRACKET_BRACE) { 
                // TODO: report error (end of pair without ',' or '}')
                JSON_RETURN_PARSER_ERROR(this, array, "Expected ',' or ']' at line: %zu, but none of them found", json_parser_peek(this).line);
            }

            json_parser_advance(this); break;
        }
    }

    return array; 
}

JSON_Node json_parse_object_node(JSON_Parser *this) {
    JSON_Object object = json_parse_object(this);
    if(json_parser_has_error(this)) {
        return JSON_NONE;
    }

    return json_object_node(object);
}

JSON_Node json_parse_array_node(JSON_Parser *this) {
    JSON_Array array = json_parse_array(this);
    if (json_parser_has_error(this)) { 
        return JSON_NONE;
    }

    return json_array_node(array);
}


JSON_Node json_parse_node(JSON_Parser *this) {
    if(json_parser_end_of_tokens(this)) { return JSON_NONE; }

    JSON_Token token = json_parser_peek(this);
    switch(token.kind) {
        case JSON_TOKEN_KIND_LEFT_CURLY_BRACE:
            return json_parse_object_node(this);
        case JSON_TOKEN_KIND_LEFT_BRACKET_BRACE:
            return json_parse_array_node(this);
        default:
            JSON_RETURN_PARSER_ERROR(this, JSON_NONE, "Invalid start of JSON node at line: %zu, expected '{' or '['", token.line);
    }
}

JSON_Node json_parse(JSON_Parser *this) {
    return json_parse_node(this);
}



#endif