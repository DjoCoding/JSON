#define JSON_IMPLEMENTATION

#include "./include/json.h"

// helper function
void print_indent(int indent) {
    for (int i = 0; i < indent; i++) printf("\t");
}

void json_format(JSON *json, int indent) {
    printf("{\n");

    for (int i = 0; i < json->count; ++i) {
        print_indent(indent + 1);

        printf("%s: ", json->items[i]->key);

        if (json->items[i]->type == NODE_TYPE_NUMBER) { printf("%ld", json->items[i]->value.as_int); }
        else if (json->items[i]->type == NODE_TYPE_STRING) { printf("%s", json->items[i]->value.as_str); }
        else { json_format(json->items[i]->value.as_json, indent + 1); }
        
        printf(",\n");
    }

    print_indent(indent);
    printf("}");
}

void json_to_string(JSON *json) {
    json_format(json, 0);
    printf("\n");
} 

void json_destroy(JSON *json) {
    if (!json) return;
    if (!json->items) return;

    for (size_t i = 0; i < json->count; ++i) {
        if (json->items[i]->type == NODE_TYPE_JSON) {
            json_destroy(json->items[i]->value.as_json);
        } else if (json->items[i]->type == NODE_TYPE_STRING) {
            free(json->items[i]->value.as_str);
        }
    }

    free(json->items);
}