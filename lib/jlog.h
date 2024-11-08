#ifndef JSON_LOG_H
#define JSON_LOG_H

#include "./jnode.h"

void _json_log_object(FILE *f, JSON_Object *this, size_t indent);
void _json_log_array(FILE *f, JSON_Array *this, size_t indent);

void pindent(FILE *f, size_t count) {
    for(size_t i = 0; i < count; ++i) {
        fprintf(f, "\t");
    }
}

void _json_log_pair_value(FILE *f, JSON_Pair_Value value, size_t indent) {
    switch(value.kind) {
        case JSON_PAIR_KIND_STRING:
            fprintf(f, "\"%s\"", value.as.string);
            break;
        case JSON_PAIR_KIND_INTEGER:
            fprintf(f, "%ld", value.as.integer);
            break;
        case JSON_PAIR_KIND_OBJECT:
            _json_log_object(f, value.as.node->as.object, indent);
            break;
        case JSON_PAIR_KIND_ARRAY:
            _json_log_array(f, value.as.node->as.array, indent);
            break;
    }
}

void _json_log_pair(FILE *f, JSON_Pair *this, size_t indent) {
    pindent(f, indent);
    fprintf(f, "\"%s\": ", this->key);
    _json_log_pair_value(f, this->value, indent);
} 

void _json_log_object(FILE *f, JSON_Object *this, size_t indent) {
    fprintf(f, "{\n");

    for(size_t i = 0; i < this->count; ++i) {
        _json_log_pair(f, &this->items[i], indent + 1);
        if (i < this->count - 1) {
            fprintf(f, ",");
        }
        fprintf(f, "\n");
    }

    pindent(f, indent);
    fprintf(f, "}");
}

void _json_log_array(FILE *f, JSON_Array *this, size_t indent) {
    fprintf(f, "[\n");

    for(size_t i = 0; i < this->count; ++i) {
        pindent(f, indent + 1);
        _json_log_object(f, &this->items[i], indent + 1);
        if (i < this->count - 1) {
            fprintf(f, ",");
        }
        fprintf(f, "\n");
    }

    pindent(f, indent);
    fprintf(f, "]");
}

void json_log_object(FILE *f, JSON_Object *this) {
    _json_log_object(f, this, 0);
    fprintf(f, "\n");
} 

void json_log_array(FILE *f, JSON_Array *this) {
    _json_log_array(f, this, 0);
    fprintf(f, "\n");
}

void json_log_node(FILE *f, JSON_Node *this) {
    switch(this->kind) {
        case JSON_NODE_KIND_OBJECT:
            json_log_object(f, this->as.object);
            break;
        case JSON_NODE_KIND_ARRAY:
            json_log_array(f, this->as.array);
            break;
        default:
            break;
    }
}


#endif