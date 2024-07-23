#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>

#define EXIT_ERROR(...) { fprintf(stderr, __VA_ARGS__); exit(EXIT_FAILURE); }
#define RETURN_ERROR(R, ...) { fprintf(stderr, __VA_ARGS__); return R; }

#define DA_INIT(arr, item_size) \
    do { \
        (arr)->count = 0; \
        (arr)->size = 1; \
        (arr)->items = malloc(item_size * (arr)->size); \
        if (!(arr)->items) EXIT_ERROR("Couldn't allocate memory\n"); \
    } while(0)

#define DA_RESIZE(arr) \
    do { \
        (arr)->size *= 2; \
        (arr)->items = realloc((arr)->items, sizeof((arr)->items[0]) * (arr)->size); \
        if (!(arr)->items) EXIT_ERROR("Couldn't reallocate memory\n"); \
    } while(0)

#define DA_FULL(arr) ((arr)->count >= (arr)->size)

#define DA_APPEND(arr, item) \
    do { \
        if (DA_FULL(arr)) DA_RESIZE(arr); \
        (arr)->items[(arr)->count++] = item; \
    } while(0)


char *fcontent(char *filename) {
    if(filename == NULL) EXIT_ERROR("Couldn't read the file, the filename is NULL\n");
    
    FILE *f = fopen(filename, "r");
    if (!f) EXIT_ERROR("Couldn't open the file: %s\n", filename);

    fseek(f, 0, SEEK_END);
    size_t fsize = ftell(f);
    fseek(f, 0, SEEK_SET);

    char *content = (char *)malloc(sizeof(char) * (fsize + 1));
    
    size_t size_read = fread(content, sizeof(char), fsize, f);

    content[size_read] = '\0';

    return content;
}


#endif // UTILS_H
