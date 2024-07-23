#ifndef JSON_DEFS_H
#define JSON_DEFS_H

#include <stdio.h>

struct _JSON_Object;
struct _JSON_Array;

enum _JSON_Node_Value_Type {
    JSON_NODE_TYPE_STRING,
    JSON_NODE_TYPE_NUMBER,
    JSON_NODE_TYPE_JSON_OBJECT,
    JSON_NODE_TYPE_JSON_ARRAY
};
typedef enum _JSON_Node_Value_Type JSON_Node_Value_Type;

union _JSON_Node_Value {
    char *as_string;
    long as_number;
    struct _JSON_Object *as_json_object;
    struct _JSON_Array *as_json_array;
};
typedef union _JSON_Node_Value JSON_Node_Value;

struct _JSON_Node {
    char *key;
    JSON_Node_Value_Type type;
    JSON_Node_Value value;
};
typedef struct _JSON_Node JSON_Node;

struct _JSON_Object {
    JSON_Node **items;
    size_t count;
    size_t size;
};
typedef struct _JSON_Object JSON_Object;

struct _JSON_Array {
    JSON_Object **items;
    size_t count;
    size_t size;
};
typedef struct _JSON_Array JSON_Array;


#endif