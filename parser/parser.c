#define PARSER_IMPLEMENTATION

#include "./parser.h"

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

void print_indent(int indent) {
    for (int i = 0; i < indent; i++) printf("\t");
}

void json_format(JSON *json, int indent) {
    printf("{\n");

    for (int i = 0; i < json->count; ++i) {
        print_indent(indent + 1);

        printf("%s: ", json->items[i]->key);

        if (json->items[i]->type == NODE_TYPE_NUMBER) { printf("%d", json->items[i]->value.as_int); }
        else if (json->items[i]->type == NODE_TYPE_STRING) { printf("%s", json->items[i]->value.as_str); }
        else { json_format(json->items[i]->value.as_json, indent + 1); }
        
        printf(",\n");
    }

    print_indent(indent);
    printf("}");
}

void json_to_string(JSON *json) {
    json_format(json, 0);
    printf("\n");
} 