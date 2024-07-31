#include "json.h"

int main(void) {
    char *file_content = fcontent("./file.json");
    
    JSON_Node *root = JSON_parse(file_content);

    JSON_log(root);

    JSON_Node *find_node = JSON_get_value(root, "djo");

    JSON_log(find_node);

    JSON_destroy(&root);

    return 0;
}