#include "./include/lexer.h"
#include "./include/lib.h"
#include "./include/parser.h"
#include "./include/json_modifier.h"
#include "./include/json.h"

JSON *toJSON(char *filename) {
    char *content = fcontent(filename);
    Lexer lexer = lexer_init(content);
    Tokens tokens = lex_json_file(&lexer);
    Parser parser = parser_init(tokens);
    JSON *json = parse(&parser);
    free(tokens.items);
    free(content);
    return json;
}

void json_node_print(JSON_Node *node) {
    if (node == NULL) return;
    if (node->type == NODE_TYPE_JSON) { json_to_string(node->value.as_json); return; }
    if (node->type == NODE_TYPE_STRING) { printf("%s", node->value.as_str); return; }
    if (node->type == NODE_TYPE_NUMBER) { printf("%ld", node->value.as_int); return; }
    fprintf(stderr, "ERROR: invalid node type\n");
    exit(EXIT_FAILURE);
}

int main(void) {    
    JSON *json = toJSON("./file.json");

    JSON_Node *node = json_get(json, "/.languages.low-level-languages");

    json_node_print(node);
    printf("\n");

    json_destroy(json);

    return 0;
}