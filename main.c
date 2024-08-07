#define JSON_IMPLEMENTATION
#include "json.h"

JSON_String generate_file(char *filename, size_t file_size, char *file_permissions) {
    JSON_String file = {0};

    json_string_begin(&file);
    {
        json_string_begin_block(&file);
        json_string_append_string(&file, "name", filename);
        json_string_append_integer(&file, "size", file_size);
        json_string_append_string(&file, "permissions", file_permissions);
        json_string_end_block(&file);
    }
    json_string_end(&file);

    return file;
}

int main2(void) {
    JSON object = json_parse_file("./file.json");
    if (!object) { errtell(); }
    json_clean_object(object);
    return 0;
}

int main(void) {
    JSON_String dir = {0};

    json_string_begin(&dir);
    {
        json_string_begin_block(&dir);
        json_string_append_object_and_free(&dir, "first", generate_file("file.json", 220, "rwx"));
        json_string_end_block(&dir);
    }
    json_string_end(&dir);

    // re-open the json string and append more items
    json_string_begin(&dir);
    {
        json_string_append_object_and_free(&dir, "second", generate_file("README.md", 102, "rw"));
        json_string_end_block(&dir);
    }
    json_string_end(&dir);


    // log the json string
    json_string_log(dir);

    JSON object;
    if ((object = json_parse_json_string(dir)) == NULL) {
        errtell();
        json_string_clean(dir);
        return 1;
    }

    // get values for the object
    char *file_name_of_first_file = NULL;
    if (!json_extract(json_get(object, ".first.name"), (void *)&file_name_of_first_file)) {
        printf("field not found\n");
    } else { printf("value is: %s\n", file_name_of_first_file); };

    // log the json object to stdout
    JSON_Log(object);

    // write the object to jsno file
    json_write_object_to_file("output.json", object);

    json_string_clean(dir);
    json_clean_object(object);

    return 0;
}