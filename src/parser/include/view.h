#ifndef VIEW_H
#define VIEW_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct {
    char *content;
    size_t size;
} View;

View view_init(char *content);
char *view_to_string(View view);


#endif