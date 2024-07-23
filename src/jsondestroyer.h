#ifndef JSON_DESTROYER_H
#define JSON_DESTROYER_H

#include "jsondefs.h"

void JSON_destroy_string_node(char *s) {
    free(s);
}

void JSON_destroy_json_node(JSON_Node **node);

void JSON_destroy_object_node(JSON_Object *object) {
    for (size_t i = 0; i < object->count; ++i) {
        JSON_Node *node = object->items[i];
        JSON_destroy_json_node(&node);
    }
    free(object->items);
    free(object);
}

void JSON_destroy_array_node(JSON_Array *array) {
    for (size_t i = 0; i < array->count; ++i) {
        JSON_Object *object = array->items[i];
        JSON_destroy_object_node(object);
    }
    free(array->items);
    free(array);
}

void JSON_destroy_json_node(JSON_Node **node) {
    if (*node == NULL) return;

    if ((*node)->key)
        free((*node)->key);

    if ((*node)->type == JSON_NODE_TYPE_STRING) {
        JSON_destroy_string_node((*node)->value.as_string);
    } else if ((*node)->type == JSON_NODE_TYPE_JSON_ARRAY) {
        JSON_destroy_array_node((*node)->value.as_json_array);
    } else if ((*node)->type == JSON_NODE_TYPE_JSON_OBJECT) {
        JSON_destroy_object_node((*node)->value.as_json_object);
    } else {
        EXIT_ERROR("'JSON_destroy_json_node' failed, JSON node type not known\n");
    }

    free(*node);
    *node = NULL;
}

void JSON_destroy(JSON_Node **node) {
    JSON_destroy_json_node(node);
}

#endif // JSON_DESTROYER_H