#ifndef JSON_DEFS_H
#define JSON_DEFS_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "../tools/sv.h"

typedef struct Location Location;

struct Location {
    size_t row;
    size_t col;
};

#define LOC_FMT           "(%zu:%zu)"
#define LOC_UNWRAP(loc)   loc.row, loc.col


// The json string sturcture

typedef struct JSON_String JSON_String;

struct JSON_String {
    char *content;
    size_t count;
    size_t size;
};

// The json object nodes

typedef enum JSON_Node_Kind JSON_Node_Kind;
typedef union JSON_Node_As JSON_Node_As;
typedef struct JSON_Item JSON_Item;
typedef struct JSON_Object JSON_Object;
typedef JSON_Object *JSON; 

enum JSON_Node_Kind {
    JSON_NODE_KIND_STRING = 0,
    JSON_NODE_KIND_INTEGER,
    JSON_NODE_KIND_FLOAT,
    JSON_NODE_KIND_OBJECT,
};  

union JSON_Node_As {
    String_View _string;
    int64_t _integer;
    double _float;
    JSON _object;
};

struct JSON_Item {
    String_View key;
    JSON_Node_Kind kind;
    JSON_Node_As as;
};

struct JSON_Object {
    JSON_Item *items;
    size_t count;
    size_t size;
};

// the json string tokens

typedef enum JSON_Token_Kind JSON_Token_Kind;
typedef struct JSON_Token JSON_Token;
typedef struct JSON_Token_List JSON_Token_List;
typedef struct JSON_Source_File JSON_Source_File;
typedef struct JSON_Lexer JSON_Lexer;
typedef struct JSON_Error JSON_Error;
typedef struct JSON_Parser JSON_Parser;

enum JSON_Token_Kind {
    TOKEN_KIND_LEFT_CURLY_BRACE = 0,   // {
    TOKEN_KIND_RIGHT_CURLY_BRACE,      // }
    TOKEN_KIND_COMMA,                  // ,
    TOKEN_KIND_COLON,                  // :
    TOKEN_KIND_STRING,                 // "string"
    TOKEN_KIND_INTEGER,                // 1992 
    TOKEN_KIND_FLOAT,                  // 10002.10
};

struct JSON_Token {
    JSON_Token_Kind kind;
    String_View text;
    Location loc;
};

struct JSON_Token_List {
    JSON_Token *items;
    size_t count;
    size_t size;
};

struct JSON_Source_File {
    char *filename;
    JSON_String text;
};

struct JSON_Lexer {
    size_t current;
    Location loc;
};

struct JSON_Error {
    bool has_error;
    char *message;
};  

struct JSON_Parser {
    JSON_Source_File source;
    JSON_Token_List tokens;
    JSON root;
    JSON_Error error;
    JSON_Lexer lexer;
    size_t curr_token;
};

// the json path 

typedef struct String_View_List JSON_Path;

struct String_View_List {
    String_View *items;
    size_t count;
    size_t size;
};

#endif // JSON_DEFS_H