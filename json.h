#ifndef JSON_H
#define JSON_H

#include "./lib/jerror.h"
#include "./lib/jtoken.h"
#include "./lib/jlexer.h"
#include "./lib/jnode.h"
#include "./lib/jparser.h"
#include "./lib/jlog.h"

typedef struct {
    char*       content;
    JSON_Lexer  lexer;
    JSON_Tokens tokens;
    JSON_Parser parser;
    JSON_Node   node;
    JSON_Error  error;
} JSON_Maker;

typedef struct {
    JSON_Node  node;
    JSON_Error error;
} JSON_Result;

JSON_Result json(char *content) {
    JSON_Maker maker = {0};
    maker.content    = strdup(content);
    maker.lexer      = json_lexer(maker.content);

    maker.tokens = json_lex(&maker.lexer);
    if (json_lexer_has_error(&maker.lexer)) {
        maker.error = maker.lexer.error;
        goto defer;
    }

    maker.parser = json_parser(maker.tokens);
    maker.node   = json_parse(&maker.parser);
    if (json_parser_has_error(&maker.parser)) {
        maker.error = maker.parser.error;
        goto defer;
    } 


    free(maker.content);
    json_tokens_free(&maker.tokens);
    return (JSON_Result) { .error = maker.error, .node = maker.node };

defer:
    free(maker.content);
    json_tokens_free(&maker.tokens);
    json_node_free(&maker.node);
    return (JSON_Result) { .error = maker.error, .node = JSON_NONE };
}


#endif