#ifndef JSON_CLEAN_H
#define JSON_CLEAN_H

#include "json-defs.h"

// Clean the json object (memory de-allocation)
void json_clean_object(JSON object);

// Clean the json parser
void json_parser_clean(JSON_Parser *self);

#ifdef JSON_CLEAN_IMPLEMENTATION

void json_parser_clean_tokens(JSON_Parser *self) {
    if (self->tokens.items) { free(self->tokens.items); }
}

void json_parser_clean_source(JSON_Parser *self) {
    if (self->source.text.count != 0) { free(self->source.text.content); }
}

void json_parser_clean_error(JSON_Parser *self) {
    if (self->error.message) { free(self->error.message); }
}

void json_parser_clean(JSON_Parser *self) {
    json_parser_clean_source(self);
    json_parser_clean_tokens(self);
}

void json_clean_item(JSON_Item item) {
    if (item.kind == JSON_NODE_KIND_OBJECT) {
        json_clean_object(item.as._object);
    }
}

void json_clean_object(JSON object) {
    if (!object) return;

    for (size_t i = 0; i < object->count; ++i) {
        json_clean_item(object->items[i]);
    }
}

#endif 

#endif // JSON_CLEAN_H