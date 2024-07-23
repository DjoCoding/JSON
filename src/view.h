#ifndef VIEW_H
#define VIEW_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *content;
    size_t size;
} View;


View view_init(char *content) {
    return (View) {
        .content = content,
        .size = (content == NULL) ? 0 : strlen(content),
    };
}

char *view_to_string(View *view) {
    if(view->size == 0) return NULL;
    if(view->content == NULL) return NULL;

    char *content = (char *)malloc(sizeof(char) * (view->size + 1));
    memcpy(content, view->content, view->size * sizeof(char));

    content[view->size] = '\0';

    return content;
}

#endif // VIEW_H