#include "./lexer.h"
#include "./lib.h"
#include "./parser.h"

void print_file_content(char *content) {
    printf("FILE CONTENT: \n");
    printf("%s\n", content);
    printf("\n");
}

int main(void) {    
    char *filename = "./file.json";
    char *content = fcontent(filename);

    Lexer lexer = lexer_init(content);
    Tokens tokens = lex(&lexer);

    // for (size_t i = 0; i < tokens.count; ++i) {
    //     token_to_string(tokens.items[i]);
    // }

    Parser parser = parser_init(tokens);

    JSON *json = parse(&parser);

    json_to_string(json);


    free(tokens.items);


    free(content);
    return 0;
}