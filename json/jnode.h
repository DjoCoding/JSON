#ifndef JSON_NODES_H
#define JSON_NODES_H

#include "../utils/mem.h"
#include <stdint.h>

#define JSON_OBJECT_INITIAL_PAIR_CAPACITY  10
#define JSON_ARRAY_INITIAL_OBJECT_CAPACITY 5

typedef struct JSON_Object JSON_Object;
typedef struct JSON_Array JSON_Array;
typedef struct JSON_Node JSON_Node;

typedef enum {
    JSON_PAIR_KIND_STRING = 0,
    JSON_PAIR_KIND_INTEGER,
    JSON_PAIR_KIND_OBJECT,
    JSON_PAIR_KIND_ARRAY,
} JSON_Pair_Kind;

typedef union {
    char        *string;
    int64_t     integer;
    JSON_Node   *node;
} JSON_Pair_As;

typedef struct {
    JSON_Pair_Kind kind;
    JSON_Pair_As   as;
} JSON_Pair_Value;

typedef char * JSON_Pair_Key;

typedef struct {
    JSON_Pair_Key   key;
    JSON_Pair_Value value;
} JSON_Pair;

typedef enum {
    JSON_NODE_KIND_NONE = 0,
    JSON_NODE_KIND_OBJECT, 
    JSON_NODE_KIND_ARRAY,
} JSON_Node_Kind;

typedef union {
    JSON_Object *object;
    JSON_Array  *array;
} JSON_Node_As;

struct JSON_Node {
    JSON_Node_Kind kind;
    JSON_Node_As   as;
};

struct JSON_Object {
    JSON_Pair *items;
    size_t count;
    size_t size;
};

struct JSON_Array {
    JSON_Object *items;
    size_t count;
    size_t size;
};

JSON_Node  json_object_node(JSON_Object object);
JSON_Node  json_array_node(JSON_Array array);

JSON_Pair_Value json_pair_value(JSON_Pair_Kind kind, JSON_Pair_As as) {
    return (JSON_Pair_Value) { .kind = kind, .as = as }; 
}

JSON_Pair_Value json_pair_string_value(char *string) {
    JSON_Pair_As as = { .string = string };
    return json_pair_value(JSON_PAIR_KIND_STRING, as);
}

JSON_Pair_Value json_pair_integer_value(int64_t integer) {
    JSON_Pair_As as = { .integer = integer };
    return json_pair_value(JSON_PAIR_KIND_INTEGER, as);
}

JSON_Pair_Value json_pair_object_value(JSON_Object object) {
    JSON_Node *node = alloc(sizeof(JSON_Node));
    *node = json_object_node(object);

    JSON_Pair_As as = { .node = node };
    return json_pair_value(JSON_PAIR_KIND_OBJECT, as);
}

JSON_Pair_Value json_pair_array_value(JSON_Array array) {
    JSON_Node *node = alloc(sizeof(JSON_Node));
    *node = json_array_node(array);

    JSON_Pair_As as = { .node = node };
    return json_pair_value(JSON_PAIR_KIND_ARRAY, as);
}

JSON_Pair json_pair(JSON_Pair_Key key, JSON_Pair_Value value) {
    return (JSON_Pair) { .key = key, .value = value }; 
}

JSON_Node json_node(JSON_Node_Kind kind, JSON_Node_As as) {
    return (JSON_Node) { .kind = kind, .as = as }; 
}

#define JSON_NONE   json_node(JSON_NODE_KIND_NONE, (JSON_Node_As) {0})

JSON_Node json_object_node(JSON_Object object) {
    JSON_Object *pobject = alloc(sizeof(JSON_Object));
    *pobject = object;

    JSON_Node_As as = { .object = pobject };
    return json_node(JSON_NODE_KIND_OBJECT, as);
}

JSON_Node json_array_node(JSON_Array array) {
    JSON_Array *parray = alloc(sizeof(JSON_Array));
    *parray = array;

    JSON_Node_As as = { .array = parray };
    return json_node(JSON_NODE_KIND_ARRAY, as);
}

JSON_Object json_object() {
    JSON_Object object = {0};
    object.size = JSON_OBJECT_INITIAL_PAIR_CAPACITY;
    object.items = alloc(sizeof(JSON_Pair) * object.size);
    return object;
}

JSON_Array json_array() {
    JSON_Array jarray = {0};
    jarray.size = JSON_ARRAY_INITIAL_OBJECT_CAPACITY;
    jarray.items = alloc(sizeof(JSON_Object) * jarray.size);
    return jarray;
}

int json_object_full(JSON_Object *this) {
    return this->count >= this->size;
}

void json_object_resize(JSON_Object *this) {
    this->size *= 2;
    this->items = ralloc(this->items, sizeof(JSON_Pair) * this->size);
}

void json_object_append_pair(JSON_Object *this, JSON_Pair pair) {
    if (json_object_full(this)) { json_object_resize(this); }
    this->items[this->count++] = pair;
}

int json_array_full(JSON_Array *this) {
    return this->count >= this->size;
}

void json_array_resize(JSON_Array *this) {
    this->size *= 2;
    this->items = ralloc(this->items, sizeof(JSON_Object) * this->size);
}

void json_array_append_object(JSON_Array *this, JSON_Object object) {
    if (json_array_full(this)) { json_array_resize(this); }
    this->items[this->count++] = object;
}


#endif