#define PARSER_IMPLEMENTATION

#include "./include/parser.h"

Parser parser_init(Tokens tokens) {
    return (Parser) {
        .tokens = tokens,
        .current = 0,
    };
}

Token parser_peek(Parser *parser) {
    return (parser->tokens.items[parser->current]);
}

void parser_consume(Parser *parser) {
    parser->current++;
}

int parser_end(Parser *parser) {
    return (parser->current >= parser->tokens.count);
}

int parser_expect(Parser *parser, Token_Type type) {
    if (parser_end(parser)) return 0;
    return (parser_peek(parser).type == type);
}

JSON_Node *json_node_init(JSON_Node node) {
    JSON_Node *ptr_node = malloc(sizeof(JSON_Node));
    if (ptr_node == NULL) {
        fprintf(stderr, "Could not allocate memory for a json node\n");
        exit(EXIT_FAILURE);
    }

    *ptr_node = node;

    return ptr_node;
}

JSON *json_init(JSON json) {
    JSON *ptr = malloc(sizeof(JSON));
    if (ptr == NULL) {
        fprintf(stderr, "Could not allocate memory for a json node\n");
        exit(EXIT_FAILURE);
    }

    *ptr = json;

    return ptr;
}

char *parse_string(Parser *parser) {    
    if (parser_expect(parser, TT_STRING) != 1) {
        goto defer;
    }

    Token current = parser_peek(parser);
    char *value = view_to_string(current.value);

    parser_consume(parser);

    return value;

defer:
    parser->error = PARSER_UNEXPECTED_TOKEN;
    return NULL;
}

long parse_number(Parser *parser) {
    if (parser_expect(parser, TT_NUMBER) != 1) {
        goto defer;
    }

    Token current = parser_peek(parser);
    char *string_value = view_to_string(current.value);
    long value = atol(string_value);
    free(string_value);

    parser_consume(parser);

    return value;

defer:
    parser->error = PARSER_UNEXPECTED_TOKEN;
    return 0;
}

JSON_Node *parse_json_node(Parser *parser) {
    // json node format 
    // { 
    // key: string
    // value: can be a string, a number, a json
    // }

    if (parser_expect(parser, TT_STRING) != 1) {
        goto defer;
    }

    // initializing the json node
    JSON_Node node = {0};

    // getting the current token
    Token current = parser_peek(parser);

    // putting the key in its place
    node.key = view_to_string(current.value);

    // consuming the key token
    parser_consume(parser);

    // expect a colon
    if (parser_expect(parser, TT_COLON) != 1) {
        goto defer;
    }

    // if found, consume it
    parser_consume(parser);

    if (parser_end(parser)) {
        goto defer;
    }

    // parse the value
    current = parser_peek(parser);
    switch(current.type) {
        case TT_NUMBER:
            node.value.as_int = parse_number(parser);
            node.type = NODE_TYPE_NUMBER;
            break;
        case TT_STRING:
            node.value.as_str = parse_string(parser);
            node.type = NODE_TYPE_STRING;
            break;
        case TT_LEFTB:
            node.value.as_json = parse(parser);
            node.type = NODE_TYPE_JSON;
            break;
    }   

    // if we encounter ',' that means we got more tokens to parse in that json
    // let the parse function handle this 
    

    // if we parse an object we expect the } token
    // also let the parse function handle it


    return json_node_init(node);

defer:
    parser->error = PARSER_UNEXPECTED_TOKEN;
    return NULL;
}   

JSON *parse(Parser *parser) {    
    JSON json = {0};

    if (parser_expect(parser, TT_LEFTB) != 1) {
        goto defer;
    }

    // consume the {
    parser_consume(parser);

    while (!parser_end(parser)) {
        JSON_Node *node = parse_json_node(parser);
        
        if (parser->error != PARSER_NO_ERROR) {
            printf("found error\n");
            exit(EXIT_FAILURE);
        }

        DA_APPEND(&json, node);

        // if ',' is found iterate one more time
        if (parser_expect(parser, TT_COMMA)) {
            parser_consume(parser);
            continue;
        } 

        // if '}' is found then return the json object
        if (parser_expect(parser, TT_RIGHTB)) {
            parser_consume(parser);
            break;
        }
    }

    return json_init(json);

defer:
    parser->error = PARSER_UNEXPECTED_TOKEN;
    return NULL;
}

int get_field_index(JSON *json, char *key) {
    for (int i = 0; i < (int)json->count; ++i) {
        if (strcmp(key, json->items[i]->key) == 0) { return i; }
    }
    return -1;
}

JSON_Node *get_field(Parser *parser, JSON *root) {
    // check for the point token
    if (parser_expect(parser, TT_POINT) != 1) {
        fprintf(stderr, "ERROR: expected a point token\n");
        exit(EXIT_FAILURE);
    }

    parser_consume(parser);

    if (parser_expect(parser, TT_FIELD) != 1) {
        fprintf(stderr, "ERROR: expected a field token\n");
        exit(EXIT_FAILURE);
    }

    Token current = parser_peek(parser);
    parser_consume(parser);

    // search for the field key in the items of the current json node
    char *field_key = view_to_string(current.value);
    int index = get_field_index(root, field_key);

    if (index == -1) {
        fprintf(stderr, "ERROR: json type has no field key %s\n", field_key);
        exit(EXIT_FAILURE);
    }

    free(field_key);

    return root->items[index];
}

JSON_Node *json_get_json_node(JSON *obj, Tokens tokens) {
    if (obj == NULL) { return NULL; }
    if (obj->items == NULL) { return NULL; }

    Parser parser = parser_init(tokens);

    if (parser_expect(&parser, TT_ROOT) != 1) {
        fprintf(stderr, "ERROR: expected the root token first\n");
        exit(EXIT_FAILURE);
    }

    // consume the root token
    parser_consume(&parser);
    
    JSON *root = obj;
    JSON_Node *node = NULL;

    // parse a field
    while (!parser_end(&parser)) {
        node = get_field(&parser, root);
        if (node->type != NODE_TYPE_JSON) { break; }
        root = node->value.as_json;
    }

    if (!parser_end(&parser)) {
        fprintf(stderr, "ERORR: unexpected token found\n");
        exit(EXIT_FAILURE);
    }

    return node;
}

void json_set_json_node(JSON **obj, Tokens tokens, JSON_Node *item) {
    JSON_Node *node = json_get_json_node(obj, tokens);
    if (node->type == NODE_TYPE_JSON) { free(node->value.as_json); }
    else if (node->type == NODE_TYPE_STRING) { free(node->value.as_str); }
}