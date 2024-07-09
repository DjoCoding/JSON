#ifndef JSON_H
#define JSON_H

#include "./lib.h"

typedef struct JSON_Format JSON;

typedef union {
    int as_int;
    JSON *as_json;
} Pair_Value;   

typedef enum {
    INTEGER_TYPE,
    JSON_TYPE,
} Pair_Value_Type;

typedef struct {
    char *key;
    Pair_Value_Type type;
    Pair_Value value;
} Pair;

struct JSON_Format {    
    Pair *items;
    size_t count;
    size_t size;
};


JSON *json_init();
void json_add_item(JSON *json, Pair item);
void json_remove_item(JSON *json, char *key);
void json_to_string(JSON *json);
void json_destroy(JSON **json);


#endif