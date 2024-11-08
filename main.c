#include <stdio.h>

#include "./json.h"

char *fcontent(const char *filepath) {
    FILE *f = fopen(filepath, "r");
    
    if (!f) { 
        perror("failed to open the file");
        exit(EXIT_FAILURE);
    }

    fseek(f, 0, SEEK_END);
    size_t fsize = ftell(f);
    fseek(f, 0, SEEK_SET);


    char *content = malloc(sizeof(char) * (fsize + 1));
    fread(content, sizeof(char), fsize, f);
    content[fsize] = 0;

    fclose(f);

    return content;
}

int main(void) {
    const char *filepath = "./example.json";
    char *content = fcontent(filepath);
    
    JSON_Result result = json(content);
    if (result.error.has_error) {
        fprintf(stderr, "failed to parse the json file\n");
        fprintf(stderr, "ERROR: %s\n", result.error.error);
        exit(EXIT_FAILURE);
    }

    JSON_Node node = result.node;
    json_log_node(stdout, &node);
    json_node_free(&node);

    free(content);
    return 0;
}