#ifndef JSON_PATH_H
#define JSON_PATH_H

#include "json-defs.h"
#include "../tools/dyn-arr.h"

// Get a path to json item
int json_make_path(String_View s, JSON_Path *path);

#ifdef JSON_PATH_IMPLEMENTATION

int json_make_path(String_View s, JSON_Path *path) {
    if (s.content == NULL || s.count == 0) { return false; }
    
    char *begin = s.content;
    size_t size = 0;

    DA_INIT(path, sizeof(String_View));

    while(s.count != 0) {
        if(begin[size] == '.') {
            if (size != 0) {
                DA_APPEND(path, SV_GET(begin, size));
            }
            begin += size + 1;
            size = 0;
        } else {
            size++;
        }
        s.count--;
    }

    if (size != 0) {
        DA_APPEND(path, SV_GET(begin, size));
    }

    return true;
}

#endif

#endif // JSON_PATH_H