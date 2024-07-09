#define JSON_IMPLEMENTATION

#include "./json.h"
#include <stdlib.h>
#include <string.h>

JSON *json_init() {
    JSON *json = (JSON *)malloc(sizeof(JSON));
    if (json == NULL) { exit(EXIT_FAILURE); }
    *json = (JSON) {
        .items = NULL,
        .count = 0,
        .size = 0,
    };
    return json;
}

void json_resize(JSON *json) {
    json->size = (json->size == 0) ? 2 : (json->size * 2);
    json->items = (Pair *)realloc(json->items, json->size * sizeof(Pair));
    if (json->items == NULL) {
        fprintf(stderr, "ERROR: could not resize the json variable\n");
        exit(EXIT_FAILURE);
    }
}

int json_is_full(JSON *json) {
    return (json->count >= json->size);
}

// start simple then make it complex
void json_add_item(JSON *json, Pair item) {    
    if (json_is_full(json)) { json_resize(json); }
    json->items[json->count++] = item;
}

// returns the index of the item if found else return -1
int json_find_item(JSON *json, char *key) {
    for (int i = 0; i < json->count; ++i) {
        if (strcmp(json->items[i].key, key) == 0) return i;
    }
    return -1;
}

void json_remove_item(JSON *json, char *key) {
    int index = json_find_item(json, key);
    if (index == -1) return;

    // remove all the item 
    if (json->items[index].type == JSON_TYPE) {
        json_destroy(&json->items[index].value.as_json);
    }

    // erase the item
    for (int i = index; i < json->count - 1; ++i) {
        json->items[i] = json->items[i + 1];
    }

    json->count--;
}

void json_destroy(JSON **json) {
    for (int i = 0; i < (*json)->count; i++) {
        if ((*json)->items[i].type == JSON_TYPE) {
            json_destroy(&((*json)->items[i].value.as_json));
        }
    }

    free((*json)->items);
    free(*json);
    
    *json = NULL;
}

void json_format(JSON *json, int indent) {
    printf("{\n");

    for (int i = 0; i < json->count; ++i) {
        print_indent(indent + 1);

        printf("%s: ", json->items[i].key);

        if (json->items[i].type == INTEGER_TYPE) { printf("%d", json->items[i].value.as_int); }
        else { json_format(json->items[i].value.as_json, indent + 1); }
        
        printf(",\n");
    }

    print_indent(indent);
    printf("}");
}

void json_to_string(JSON *json) {
    json_format(json, 0);
    printf("\n");
} 
