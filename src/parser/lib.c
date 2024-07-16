#define LIB_IMPLEMENTATION

#include "./include/lib.h"

char *fcontent(char *filename) {
    FILE *f = fopen(filename, "r");

    if (!f) {
        fprintf(stderr, "ERROR: could not open file %s for reading\n", filename);
        exit(EXIT_FAILURE);
    }

    fseek(f, 0, SEEK_END);
    size_t fsize = ftell(f);
    fseek(f, 0, SEEK_SET);

    char *content = (char *)malloc(sizeof(char) * (fsize + 1));

    size_t read_size = fread(content, sizeof(char), fsize, f);

    content[read_size] = '\0';

    fclose(f);

    return content;
}

int isbrace(char c) {
    return (c == '{' || c == '}');
}

int iscomma(char c) {
    return (c == ',');
}

int iscolon(char c) {
    return (c == ':');
}

int isquote(char c) {
    return (c == '"');
}

int ispoint(char c) {
    return (c == '.');
}

int isroot(char c) {
    return (c == '/');
}