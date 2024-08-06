#ifndef JSON_STRING_H
#define JSON_STRING_H

#include "json-defs.h"

#define JSON_STRING_INITIAL_SIZE 1024

// Open the json structure (initialze it if empty)
void json_string_begin(JSON_String *s);

// Start a json block
void json_string_begin_block(JSON_String *s);

// End the json bloc
void json_string_end_block(JSON_String *s);

// End the json structure (recommended to avoid memoty leaks)
void json_string_end(JSON_String *s);

// Append a string item to the json object string (example: "author": "Bouhadda Mohammed Djaoued")
void json_string_append_string(JSON_String *s, char *key, char *value);

// Append an integer item to the json object string (example "current-year": 2024)
void json_string_append_integer(JSON_String *s, char *key, int64_t value);

// Append a float item to the json object string (example "pi": 3.1415)
void json_string_append_float(JSON_String *s, char *key, double f);

// Append a json string to the json object string 
void json_string_append_object(JSON_String *s, char *key, JSON_String t);

// Append a json string to the json object string then clean the child json string
void json_string_append_object_and_free(JSON_String *s, char *key, JSON_String t);

// Log the json string
void json_string_log(JSON_String s);

// Get a json string from a json object
JSON_String json_string_from_object(JSON object);

// Get a deep copy of a json string
JSON_String json_string_copy(JSON_String s);

// Clean the json string structure
void json_string_clean(JSON_String s);

// Log the json string
void json_string_log(JSON_String s);

#ifdef JSON_STRING_IMPLEMENTATION

JSON_String json_string_init() {
    char *content = malloc(sizeof(char) * JSON_STRING_INITIAL_SIZE);
    if (!content) { EXIT_ERROR("could not initialize the json string due to memory allocation error\n") };
    return (JSON_String) {
        content,
        0,
        JSON_STRING_INITIAL_SIZE,
    };
}

void json_string_clean(JSON_String s) {
    if (s.content) free(s.content);
}

void json_string_resize(JSON_String *s) {
    s->size *= 2;
    s->content = realloc(s->content, s->size * sizeof(char));
    if (!s->content) { EXIT_ERROR("could not resize the json string due to memory allocation error\n"); }
}

void json_string_resize_if_needed(JSON_String *s, size_t size) {
    while (s->count + size > s->size) { json_string_resize(s); }
}

// write a c string to the json string
void json_string_write(JSON_String *s, char *t) {
    size_t t_length = strlen(t);
    json_string_resize_if_needed(s, t_length);
    memcpy(s->content + s->count, t, t_length);
    s->count += t_length;
}

void json_string_write_quote(JSON_String *s) {
    json_string_write(s, "\"");
}

void json_string_write_colon(JSON_String *s) {
    json_string_write(s, ": ");
}

void json_string_write_comma(JSON_String *s) {
    json_string_write(s, ",");
}

void json_string_begin(JSON_String *s) {
    if (s->count == 0) {
        *s = json_string_init();
        return;
    } 

    s->count--;

    if (s->content[s->count] == '}') {
        json_string_write_comma(s);
        return;
    }

    EXIT_ERROR("can not begin a json string if it wasn't closed\n");
}

// take string and write "string"
void json_string_write_string(JSON_String *s, char *t) {
    json_string_write_quote(s);
    json_string_write(s, t);
    json_string_write_quote(s);
}

void json_string_append_string(JSON_String *s, char *key, char *value) {
    json_string_write_string(s, key);
    json_string_write_colon(s);
    json_string_write_string(s, value);
    json_string_write_comma(s);
}

void json_string_append_integer(JSON_String *s, char *key, int64_t integer) {
    char value[1024];
    sprintf(value, "%ld", integer);
    json_string_write_string(s, key);
    json_string_write_colon(s);
    json_string_write(s, value);
    json_string_write_comma(s);
}

void json_string_append_float(JSON_String *s, char *key, double f) {
    char value[1024];
    sprintf(value, "%lf", f);
    json_string_write_string(s, key);
    json_string_write_colon(s);
    json_string_write(s, value);
    json_string_write_comma(s);
}

void json_string_append_object(JSON_String *s, char *key, JSON_String t) {
    char value[t.count + 1];
    sprintf(value, "%.*s", (int)t.count, t.content);

    json_string_write_string(s, key);
    json_string_write_colon(s);
    json_string_write(s, value);
    json_string_write_comma(s);
}

void json_string_append_object_and_free(JSON_String *s, char *key, JSON_String t) {
    json_string_append_object(s, key, t);
    json_string_clean(t);
}

void json_string_begin_block(JSON_String *s) {
    json_string_write(s, "{");
}  

// at each addition of an item we add ',' at the end, ending a json block means removing the last ',' and adding the '}' token
void json_string_end_block(JSON_String *s) {
    // removing the ','
    s->count--;
    
    // support for empty objects in json strings
    if (s->content[s->count] != ',') { s->count++; }

    json_string_write(s, "}");
}

void json_string_end(JSON_String *s) {
    // prevent memory leak
    s->content = realloc(s->content, s->count);
    s->size = s->count;
}

JSON_String json_string_from_sv(String_View s) {
    return (JSON_String) {
        s.content,
        s.count,
        s.count
    };
}

JSON_String json_string_copy(JSON_String s) {
    JSON_String result = {0};
    result.count = s.count;
    result.size = s.size;
    result.content = malloc(sizeof(char) * result.size);
    memcpy(result.content, s.content, result.size);
    return result;
}

void json_string_log(JSON_String s) {
    printf("%.*s\n", (int)s.count, s.content);
}

void json_string_append_string_item(JSON_String *s, JSON_Item item) {
    char *value = cstr_from_sv(item.as._string);
    char *key = cstr_from_sv(item.key);
    json_string_append_string(s, key, value);
    free(key);
    free(value);
} 

void json_string_append_integer_item(JSON_String *s, JSON_Item item) {
    char *key = cstr_from_sv(item.key);
    json_string_append_integer(s, key, item.as._integer);
    free(key);
} 

void json_string_append_float_item(JSON_String *s, JSON_Item item) {
    char *key = cstr_from_sv(item.key);
    json_string_append_float(s, key, item.as._float);
    free(key);
} 

JSON_String json_string_from_object(JSON object);

void json_string_append_object_item(JSON_String *s, JSON_Item item) {
    char *key = cstr_from_sv(item.key);
    JSON_String value = json_string_from_object(item.as._object);
    json_string_append_object_and_free(s, key, value);
    free(key);
}

void json_string_append_item(JSON_String *s, JSON_Item item) {
    switch(item.kind) {
        case JSON_NODE_KIND_STRING:
            json_string_append_string_item(s, item);
            break;
        case JSON_NODE_KIND_INTEGER:
            json_string_append_integer_item(s, item);
            break;
        case JSON_NODE_KIND_FLOAT:
            json_string_append_float_item(s, item);
            break;
        case JSON_NODE_KIND_OBJECT:
            json_string_append_object_item(s, item);
            break;
        default:
            EXIT_ERROR("unreachable");        
    }
}

JSON_String json_string_from_object(JSON object) {
    JSON_String result = {0};
    if (!object) { return result; }

    json_string_begin(&result);
    {
        json_string_begin_block(&result);
        for (size_t i = 0; i < object->count; ++i) {
            JSON_Item item = object->items[i];
            json_string_append_item(&result, item);
        }
        json_string_end_block(&result);
    }
    json_string_end(&result);

    return result;
}

#endif

#endif // JSON_STRING_H