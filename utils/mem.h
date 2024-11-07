#ifndef MEMORY_H
#define MEMORY_H

#include <stdio.h>
#include <stdlib.h>

void *alloc(size_t size) {
    void *p = malloc(size);
    if (!p) {
        fprintf(stderr, "malloc failed\n");
        exit(EXIT_FAILURE);
    }
    return p;
}

void *ralloc(void *p, size_t size) {
    void *out = realloc(p, size);
    if (!out) {
        fprintf(stderr, "realloc failed\n");
        exit(EXIT_FAILURE);
    }
    return out;
}


#endif