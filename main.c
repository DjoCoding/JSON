#include "./json.h"


int main(void) {    
    JSON *second_json = json_init();

    json_add_item(second_json, (Pair) { .key = "djaoued", .value.as_int = 10 });

    JSON *first_json = json_init();
    json_add_item(first_json, (Pair) { .key = "djaoued", .value.as_int = 10 });
    json_to_string(first_json);   

    json_add_item(first_json, (Pair) { .key = "zaki", .value.as_int = 20 });
    json_to_string(first_json);

    json_add_item(first_json, (Pair) { .key = "ryad", .type = JSON_TYPE, .value.as_json = second_json });
    json_to_string(first_json);

    json_remove_item(first_json, "ryad");
    json_to_string(first_json);

    json_destroy(&first_json);
    return 0;
}