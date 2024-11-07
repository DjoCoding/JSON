#ifndef STRING_VIEW_H
#define STRING_VIEW_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *content;
    size_t count;
} String_View;

#define SV_FMT          "%.*s"
#define SV_UNWRAP(sv)   (int) sv.count, sv.content

String_View sv(char *content, size_t count) {
    return (String_View) { content, count };
}

String_View sv_from_cstr(char *content) {
    return sv(content, strlen(content));
}

char *cstr_from_sv(String_View s) {
    char *content = malloc(sizeof(char) * (s.count+ 1));
    if (!content) { return NULL; }
    memcpy(content, s.content, s.count * sizeof(char));
    content[s.count] = 0;
    return content;
}

int64_t int_from_sv(String_View s) {
    int64_t value = 0;
    for (size_t i = 0; i < s.count; ++i) {
        value *= 10;
        value += s.content[i] - (int)'0';
    }
    return value;
}

#endif