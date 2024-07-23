#include "json.h"

int main(void) {
    char *file_content = fcontent(NULL);
    
    JSON_Node *root = JSON_parse(file_content);

    JSON_log(root);
    JSON_destroy(&root);

    return 0;
}