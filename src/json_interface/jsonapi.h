#ifndef JSON_API_H
#define JSON_API_H

#include "jsondefs.h"
#include <string.h>

JSON_Node *JSON_get_node(JSON_Node *node, char *key);


JSON_Node *JSON_Object_Search(JSON_Object *object, char *key) {
    if (object == NULL) return NULL;
    if (key == NULL) return NULL;

    for (size_t i = 0; i < object->count; ++i) {
        JSON_Node *current_node = object->items[i];
        JSON_Node *return_node = JSON_get_node(current_node, key);
        if (return_node) return return_node;
    }

    return NULL;
}

JSON_Node *JSON_get_node(JSON_Node *node, char *key) {
    if (!node) return NULL;
    if (!key) return NULL;

    if (node->key)
        if (strcmp(node->key, key) == 0) return node;

    if (node->type == JSON_NODE_TYPE_STRING || node->type == JSON_NODE_TYPE_NUMBER) return NULL;

    if (node->type == JSON_NODE_TYPE_JSON_OBJECT) return JSON_Object_Search(node->value.as_json_object, key);

    for (size_t i = 0; i < node->value.as_json_array->count; ++i) {
        JSON_Object *current_obj = node->value.as_json_array->items[i];
        JSON_Node *return_node = JSON_Object_Search(current_obj, key);
        if (return_node) return return_node;
    }

    return NULL;
}

#endif