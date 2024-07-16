#ifndef DA_H
#define DA_H

#include <stdio.h>
#include <stdlib.h>

#define DA_RESIZE(arr) \
    do { \
        if ((arr)->count < (arr)->size) break; \
        size_t new_size = ((arr)->size == 0) ? 2 : ((arr)->size * 2); \
        (arr)->items = realloc((arr)->items, sizeof((arr)->items[0]) * new_size); \
        if ((arr)->items == NULL) { \
            fprintf(stderr, "ERROR: failed to resize the array\n"); \
            exit(EXIT_FAILURE); \
        } \
        (arr)->size = new_size; \
    } while(0)


#define DA_APPEND(arr, item) \
    do { \
        DA_RESIZE(arr); \
        (arr)->items[(arr)->count++] = item; \
    } while(0)

#define DA_INIT(arr, item_size) \
    do {    \
        (arr)->size = 2; \
        (arr)->count = 0; \
        (arr)->items = malloc(item_size * (arr)->size); \
        if ((arr)->items == NULL) { \
            fprintf(stderr, "ERROR: failed to allocate the memory for the array\n"); \
            exit(EXIT_FAILURE); \
        } \
    } while(0)

#endif