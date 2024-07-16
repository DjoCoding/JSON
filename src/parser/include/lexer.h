#ifndef LEXER_H
#define LEXER_H

#include <ctype.h>

#include "./view.h"
#include "./da.h"
#include "./token.h"
#include "./lib.h"

typedef struct {
    View input;
    size_t current;
} Lexer;

Lexer lexer_init(char *input);
Tokens lex_json_file(Lexer *lexer);
Tokens lex_modifier_request(Lexer *lexer);



#endif