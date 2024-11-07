#include <stdio.h>

#include "./json/jlexer.h"
#include "./json/jparser.h"

int main(void) {
    const char *filepath = "./djaoued.json";
    FILE *f = fopen(filepath, "r");
    if (!f) { 
        perror("failed to open the json file");
        exit(EXIT_FAILURE);
    }

    fseek(f, 0, SEEK_END);
    size_t fsize = ftell(f);
    fseek(f, 0, SEEK_SET);


    char *content = malloc(sizeof(char) * (fsize + 1));
    fread(content, sizeof(char), fsize, f);
    content[fsize] = 0;

    fclose(f);

    JSON_Lexer lexer = json_lexer(content);
    JSON_Tokens tokens = json_lex(&lexer);
    if (lexer.error.has_error) { 
        fprintf(stderr, "lexing error: %s\n", lexer.error.error);
        free(tokens.items);
        exit(EXIT_FAILURE);
    }

    JSON_Parser parser = json_parser(tokens);
    JSON_Node node = json_parse(&parser);


    return 0;
}