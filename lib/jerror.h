#ifndef JSON_ERROR_H
#define JSON_ERROR_H

#include <stdio.h>

#define JSON_ERROR_STRING_CAPACITY 100

typedef struct {
    char error[JSON_ERROR_STRING_CAPACITY];
    int has_error;
} JSON_Error;

#define JSON_SET_ERROR(json_error, error_fmt, ...) \
    do { \
        json_error.has_error = 1; \
        sprintf(json_error.error, error_fmt, __VA_ARGS__); \
    } while(0) 

#endif