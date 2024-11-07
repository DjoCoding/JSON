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

JSON_Pair json_parse_pair(JSON_Parser *this) {
    assert(!json_parser_end_of_tokens(this));

    JSON_Token key_token = json_parser_peek(this);

    // TODO: handle this error (key is not string)
    assert(key_token.kind == JSON_TOKEN_KIND_STRING);

    // consume the key token
    json_parser_advance(this);

    // TODO: handle this error (key without value) 
    assert(!json_parser_end_of_tokens(this));

    // TODO: handle this error (no ':' between the key and the value) 
    assert(json_parser_peek(this).kind == JSON_TOKEN_KIND_DOUBLE_POINTS);

    // consume the ':' token
    json_parser_advance(this);

    // TODO: handle this error (key without value) 
    assert(!json_parser_end_of_tokens(this));

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
            // TODO: report error
    }

    return json_pair(cstr_from_sv(key_token.as.string), value);
}

JSON_Object json_parse_object(JSON_Parser *this) {
    JSON_Object object = json_object();

    assert(!json_parser_end_of_tokens(this));
    assert(json_parser_peek(this).kind == JSON_TOKEN_KIND_LEFT_CURLY_BRACE);

    json_parser_advance(this);

    // TODO: handle this error (start of a json object without end)
    assert(!json_parser_end_of_tokens(this));

    if (json_parser_peek(this).kind == JSON_TOKEN_KIND_RIGHT_CURLY_BRACE) { 
        json_parser_advance(this);
        return object;
    }

    while(!json_parser_end_of_tokens(this)) {
        JSON_Pair pair = json_parse_pair(this);
        json_object_append_pair(&object, pair);

        JSON_Token token = json_parser_peek(this);
        if (token.kind == JSON_TOKEN_KIND_COMMA) {
            json_parser_advance(this);
        } else {
            if (json_parser_peek(this).kind != JSON_TOKEN_KIND_RIGHT_CURLY_BRACE) { 
                // TODO: report error (end of pair without ',' or '}')
            }

            json_parser_advance(this); break;
        }
    }

    return object; 
}

JSON_Array json_parse_array(JSON_Parser *this) {
    JSON_Array array = json_array();

    assert(!json_parser_end_of_tokens(this));
    assert(json_parser_peek(this).kind == JSON_TOKEN_KIND_LEFT_BRACKET_BRACE);

    json_parser_advance(this);

    // TODO: handle this error (start of a json array without end)
    assert(!json_parser_end_of_tokens(this));

    if (json_parser_peek(this).kind == JSON_TOKEN_KIND_RIGHT_BRACKET_BRACE) { 
        json_parser_advance(this);
        return array;
    }

    while(!json_parser_end_of_tokens(this)) {
        JSON_Object object = json_parse_object(this);
        json_array_append_object(&array, object);

        JSON_Token token = json_parser_peek(this);
        if (token.kind == JSON_TOKEN_KIND_COMMA) {
            json_parser_advance(this);
        } else {
            if (json_parser_peek(this).kind != JSON_TOKEN_KIND_RIGHT_BRACKET_BRACE) { 
                // TODO: report error (end of pair without ',' or '}')
            }

            json_parser_advance(this); break;
        }
    }

    return array; 
}

JSON_Node json_parse_object_node(JSON_Parser *this) {
    JSON_Object object = json_parse_object(this);
    return json_object_node(object);
}

JSON_Node json_parse_array_node(JSON_Parser *this) {
    JSON_Array array = json_parse_array(this);
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
            assert("error: invalid begining of json file");
    }
}

JSON_Node json_parse(JSON_Parser *this) {
    return json_parse_node(this);
}



#endif