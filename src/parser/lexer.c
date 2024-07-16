#define LEXER_IMPLEMENTATION

#include "./include/lexer.h"

Lexer lexer_init(char *input) {
    return (Lexer) {
        .input = view_init(input),
        .current = 0,
    };
}   

char lexer_peek(Lexer *lexer) {
    return lexer->input.content[lexer->current];
}

void lexer_consume(Lexer *lexer) {
    lexer->current++;
}

int lexer_end(Lexer *lexer) {
    return (lexer->current >= lexer->input.size);
}

Token lexer_get_next_token(Lexer *lexer) {
    char *begin = lexer->input.content + lexer->current;

    char ch = lexer_peek(lexer);

    if (isbrace(ch)) {
        lexer_consume(lexer);
        return token_init((ch == '}' ? TT_RIGHTB : TT_LEFTB), view_init(NULL));
    } 

    if (iscomma(ch)) {
        lexer_consume(lexer);
        return token_init(TT_COMMA, view_init(NULL));
    }

    if (iscolon(ch)) {
        lexer_consume(lexer);
        return token_init(TT_COLON, view_init(NULL));
    }

    size_t size = 0; 

    if (isquote(ch)) {
        lexer_consume(lexer);
        
        // update the begin pointer to next char
        begin += 1;


        while (!lexer_end(lexer)) {
            if (isquote(lexer_peek(lexer))) { lexer_consume(lexer); break; }
            lexer_consume(lexer);
            size++;
        }

        Token_Value value = view_init(begin);
        value.size = size;

        return token_init(TT_STRING, value);
    }

    if (isdigit(ch)) {
        while (!lexer_end(lexer)) {
            if (!isdigit(lexer_peek(lexer))) { break; }
            lexer_consume(lexer);
            size++;
        }

        Token_Value value = view_init(begin);
        value.size = size;
        return token_init(TT_NUMBER, value);
    }

    fprintf(stderr, "ERROR: could not recognize this char: %c\n", ch);
    exit(EXIT_FAILURE);
}

Tokens lex_json_file(Lexer *lexer) {
    printf("[LEXER] lexing...\n");

    Tokens tokens = {0};
    
    while (!lexer_end(lexer)) {
        if (isspace(lexer_peek(lexer))) { lexer_consume(lexer); }
        else {
            Token token = lexer_get_next_token(lexer);
            DA_APPEND(&tokens, token);
        }
    }

    printf("[LEXER] file well lexed\n\n");

    return tokens;
}


Token lexer_get_next_modifier_request_token(Lexer *lexer) {
    char *begin = lexer->current + lexer->input.content;
    char ch = lexer_peek(lexer);

    if (ispoint(ch)) {
        lexer_consume(lexer);
        return token_init(TT_POINT, view_init(NULL));
    }

    if (isroot(ch)) {
        lexer_consume(lexer);
        return token_init(TT_ROOT, view_init(NULL));
    }

    if (isalpha(ch)) {
        size_t size = 0;
        while (!lexer_end(lexer)) {
            ch = lexer_peek(lexer);
            if (ispoint(ch)) { break; }
            lexer_consume(lexer);
            size++;
        }

        Token_Value value = view_init(begin);
        value.size = size;
        return token_init(TT_FIELD, value);
    }

    fprintf(stderr, "ERROR: could not recognize this char: %c\n", ch);
    exit(EXIT_FAILURE);   
}


Tokens lex_modifier_request(Lexer *lexer) {
    printf("[LEXER] lexing modifier request...\n");

    Tokens tokens = {0};

    while (!lexer_end(lexer)) {
        Token token = lexer_get_next_modifier_request_token(lexer);
        DA_APPEND(&tokens, token);
    }

    printf("[LEXER] request well lexer\n\n");
    return tokens;
}