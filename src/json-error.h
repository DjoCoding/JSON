#ifndef JSON_ERROR_H
#define JSON_ERROR_H

#include "json-defs.h"

char *error = NULL;

#define perror json_parser_error

#define MESSAGE_SIZE 1024

#define JSON_SET_ERROR(self, message) \
    do { \
        (self)->error.has_error = true; \
        (self)->error.message = message; \
    } while(0)

#define JSON_THROW_ERROR(self, ...) \
    do { \
        char *message = malloc(sizeof(char) * MESSAGE_SIZE); \
        if (!message) EXIT_ERROR("could not allocate memory"); \
        sprintf(message, __VA_ARGS__); \
        JSON_SET_ERROR(self, message); \
    } while(0)


static bool json_parser_error(JSON_Parser *self) {
    return self->error.has_error;
}


#endif // JSON_ERROR_H