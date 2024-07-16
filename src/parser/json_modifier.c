#define JSON_MODIFIER_IMPLEMENTATION

#include "include/json_modifier.h"


Tokens get_request_tokens(char *req) {
    Lexer lexer = lexer_init(req);
    Tokens tokens = lex_modifier_request(&lexer);
    return tokens;
}

JSON_Node *json_get(JSON *obj, char *req) {
    if (obj == NULL) { return NULL; }
    if (obj->items == NULL) { return NULL; }
    if (req == NULL) { return NULL; }

    Tokens tokens = get_request_tokens(req);

    return json_get_json_node(obj, tokens);   
}



void json_set(JSON *obj, char *req, JSON_Node *item) {
    if (obj == NULL) { return; }
    if (req == NULL) { return; }

    Tokens tokens = get_request_tokens(req);

    json_set_json_node(obj, tokens, item);
}