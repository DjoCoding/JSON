#ifndef JSON_PRETTY_PRINTER_H
#define JSON_PRETTY_PRINTER_H

#include "json-defs.h"
#include "json-color.h"

// log the json object to a stream
void sJSON_Log(FILE *stream, JSON object);

// log the json object to the stdout
#define JSON_Log(obj) sJSON_Log(stdout, obj)

// get the string that corresponds to the token kind 
char *json_token_kind_to_cstr(JSON_Token_Kind kind);

#ifdef JSON_PRINTER_IMPLEMENTATION

char *json_token_kind_to_cstr(JSON_Token_Kind kind) {
    char *kinds[] = {
        "{",
        "}",
        ",",
        ":",
        "string",
        "integer",
        "float"
    };

    return kinds[kind];
}

void JSON_Log_Object(FILE *stream, JSON object, size_t indent);

void print_indent(FILE *stream, size_t indent) {
    for (size_t i = 0; i < indent; ++i) {
        fprintf(stream, "  ");
    }
}

void JSON_Log_String(FILE *stream, String_View s) {
    fprintf(stream, Green "\"" SV_FMT "\"" RESET, SV_UNWRAP(s));
}

void JSON_Log_Integer(FILE *stream, int64_t integer) {
    fprintf(stream, "%ld", integer);
}

void JSON_Log_Float(FILE *stream, double f) {
    fprintf(stream, "%lf" , f);
}

void JSON_Log_Item(FILE *stream, JSON_Item item, size_t indent) {

    {
        print_indent(stream, indent);
        JSON_Log_String(stream, item.key);
        fprintf(stream, ": ");
    }

    if (item.kind == JSON_NODE_KIND_STRING) {
        JSON_Log_String(stream, item.as._string);
        return;
    }

    if (item.kind == JSON_NODE_KIND_INTEGER) {
        JSON_Log_Integer(stream, item.as._integer);
        return;
    }

    if (item.kind == JSON_NODE_KIND_FLOAT) {
        JSON_Log_Float(stream, item.as._float);
        return;
    }

    if (item.kind == JSON_NODE_KIND_OBJECT) {
        JSON_Log_Object(stream, item.as._object, indent + 1);
        return;
    }

    EXIT_ERROR("unreachable\n");  
}

void JSON_Log_Object(FILE *stream, JSON object, size_t indent) {
    if (!object) { return; }
    fprintf(stream, Cyan "{\n" RESET);

    for (size_t i = 0; i < object->count; ++i) {
        JSON_Item item = object->items[i];
        JSON_Log_Item(stream, item, indent + 1);
        if (i != object->count - 1) { fprintf(stream, ","); }
        fprintf(stream, "\n");
    }

    print_indent(stream, indent);
    fprintf(stream, Cyan "}" RESET);
}

// log the json object to a stream
void sJSON_Log(FILE *stream, JSON object) {
    JSON_Log_Object(stream, object, 0);
    fprintf(stream, "\n");
}

#endif

#endif // JSON_PRETTY_PRINTER_H