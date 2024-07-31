#ifndef JSON_STRING_H
#define JSON_STRING_H

#include "jsondefs.h"

void print_indentation(size_t indent) {
    for (size_t i = 0; i < indent; ++i) 
        printf("\t");
}

void print_string_node_value(char *s) {
    printf("\"");

    if (s)
        printf("%s", s);

    printf("\"\n");
}

void print_number_node_value(long number) {
    printf("%ld\n", number);
}

void print_json_node(JSON_Node *node, size_t indent);

void print_object_node_value(JSON_Object *object, size_t indent) {
    print_indentation(indent);
    
    printf("{");

    if (object->count != 0) printf("\n");

    for (size_t i = 0; i < object->count; ++i) {
        print_json_node(object->items[i], indent + 1);
    }

    if (object->count != 0)
        print_indentation(indent);
    printf("}\n");
}

void print_array_node_value(JSON_Array *array, size_t indent) {
    printf("[");

    if (array->count != 0) 
        printf("\n");

    for (size_t i = 0; i < array->count; ++i) {
        print_object_node_value(array->items[i], indent + 1);
    }

    if (array->count != 0) 
        print_indentation(indent);
    
    printf("]\n");
}


void print_json_node(JSON_Node *node, size_t indent) {
    if (node == NULL) return;

    for (size_t i = 0; i < indent; ++i) 
        printf("\t");
    
    if (node->key)
        printf("\"%s\": ", node->key);

    switch(node->type) {
        case JSON_NODE_TYPE_STRING:
            print_string_node_value(node->value.as_string);
            break;
        case JSON_NODE_TYPE_NUMBER:
            print_number_node_value(node->value.as_number);
            break;
        case JSON_NODE_TYPE_JSON_OBJECT:
            print_object_node_value(node->value.as_json_object, indent);
            break;
        case JSON_NODE_TYPE_JSON_ARRAY:
            print_array_node_value(node->value.as_json_array, indent);
            break;
        default:    
            EXIT_ERROR("Node type not found for printing\n");
    }
}

void JSON_log(JSON_Node *node) {
    print_json_node(node, 0);
}

#endif // JSON_STRING_H