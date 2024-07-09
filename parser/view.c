#define VIEW_IMPLEMENTATION

#include "./view.h"

View view_init(char *content) {
    View view = (View) {
        .content = content,
        .size = (content == NULL) ? 0 : strlen(content),
    };

    return view;
}

char *view_to_string(View view) {
    if (view.size == 0) return NULL;
    
    char *content = (char *)malloc(sizeof(char) * (view.size + 1));
    memcpy(content, view.content, view.size * sizeof(char));
    content[view.size] = '\0';

    return content;
}
