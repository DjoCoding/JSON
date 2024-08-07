#ifndef JSON_H
#define JSON_H

#include <errno.h>

#include "src/json-error.h"
#define JSON_CLEAN_IMPLEMENTATION
#include "src/json-clean.h"
#define JSON_LEXER_IMPLEMENTATION
#include "src/json-lexer.h"
#define JSON_PARSER_IMPLEMENTATION
#include "src/json-parser.h"
#define JSON_PATH_IMPLEMENTATION
#include "src/json-path.h"
#define JSON_STRING_IMPLEMENTATION
#include "src/json-string.h"
#define JSON_PRINTER_IMPLEMENTATION
#include "src/json-pretty-printer.h"

// Get the json item from the object
JSON_Item *json_get(JSON_Object *object, char *str);

// Get the value field from an object based on its key (returns 0 for failure)
int json_extract(JSON_Item *item, void *value);

// Parse a json file 
JSON json_parse_file(char *filename);

// Parse a json string
JSON json_parse_json_string(JSON_String s);

#ifdef JSON_IMPLEMENTATION

JSON_Parser json_parser_from_file(char *filename) {
    JSON_Parser parser = {0};
    parser.source.filename = filename;
    parser.source.text = json_string_from_sv(sv_from_file(filename));
    parser.lexer.loc = (Location) {1, 1};
    return parser;
}

JSON_Parser json_parser_from_json_string(JSON_String s) {
    JSON_Parser parser = {0};
    parser.source.filename = NULL;
    parser.source.text = json_string_copy(s);
    parser.lexer.loc = (Location) {1, 1};
    return parser;
}

JSON_Item *json_find_item(JSON object, String_View field) {
    for (size_t i = 0; i < object->count; ++i) {
        JSON_Item current = object->items[i];
        if (sv_eq(current.key, field)) { return &object->items[i]; }
    }
    return NULL;
}

JSON_Item *json_search(JSON object, JSON_Path path) {
    JSON current = object;
    JSON_Item *item = NULL;

    for (size_t i = 0; i < path.count; ++i) {
        String_View field = path.items[i];
        item = json_find_item(current, field);
        if (!item) { return NULL; }
        if (i != path.count - 1 && item->kind != JSON_NODE_KIND_OBJECT) { return NULL; }
        else if (item->kind != JSON_NODE_KIND_OBJECT) { break; } 
        current = item->as._object;
    }

    return item;
}

JSON_Item *json_get(JSON object, char *str) {
    if (!object) { return NULL; }
    if (!str) { return NULL; }
    
    String_View s = SV(str);
    JSON_Path path = {0};
    
    if (!json_make_path(s, &path)) {
        return NULL;
    }

    return json_search(object, path);
}

int json_extract(JSON_Item *item, void *value) {
    if (!item) { return false; }

    if (item->kind == JSON_NODE_KIND_STRING) {
        char *v = (char *)cstr_from_sv(item->as._string);
        memcpy(value, &v, sizeof(void *));
        return 1;
    } 
    
    if(item->kind == JSON_NODE_KIND_INTEGER) {
        int64_t v = item->as._integer;
        memcpy(value, &v, sizeof(int64_t));
        return 1;
    } 

    if(item->kind == JSON_NODE_KIND_FLOAT) {
        double v = item->as._float;
        memcpy(value, &v, sizeof(double));
        return 1;
    } 

    if(item->kind == JSON_NODE_KIND_OBJECT) {
        JSON v = item->as._object;
        memcpy(value, &v, sizeof(void *));
        return 1;
    }

    return 0;
}  

JSON json_parse_using_parser(JSON_Parser *self) {
    lex(self);
    if (perror(self)) { error = self->error.message; goto out;}
    
    parse(self);
    if (perror(self)) { error = self->error.message; goto clean;}
    
    JSON result = self->root;
    return result;

clean:
    json_parser_clean(self);

out:
    return NULL;
}

JSON json_parse_file(char *filename) {
    JSON_Parser parser = json_parser_from_file(filename);
    return json_parse_using_parser(&parser);
}


JSON json_parse_json_string(JSON_String s) {
    JSON_Parser parser = json_parser_from_json_string(s);
    return json_parse_using_parser(&parser);
}

void json_write_object_to_file(char *filename, JSON object) {
    FILE *f = fopen(filename, "w");
    if (!f) {
        EXIT_ERROR("could not open the file %s: %s", filename, strerror(errno));
    }
    sJSON_Log(f, object);
    fclose(f);
}

#endif


#endif // JSON_H