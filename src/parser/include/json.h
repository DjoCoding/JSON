#ifndef JSON_H
#define JSON_H

#include <stdio.h>
#include <stdlib.h>

typedef struct JSON JSON;

typedef enum {
    NODE_TYPE_NUMBER,
    NODE_TYPE_STRING,
    NODE_TYPE_JSON,
} Node_Type;

typedef union {
    long as_int;
    char *as_str;
    JSON *as_json;
} Node_Value;

typedef struct  {
    char *key;
    Node_Type type;
    Node_Value value;
} JSON_Node;

struct JSON {
    JSON_Node **items;
    size_t count;
    size_t size;
};

void json_to_string(JSON *jso);
void json_destroy(JSON *json);

#endif