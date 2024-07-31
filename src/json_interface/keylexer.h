#ifndef JSON_KEY_LEXER
#define JSON_KEY_LEXER

#include "../view.h"

struct _Key_Lexer {
    View key;
    size_t current;
};
typedef _Key_Lexer Key_Lexer;




#endif // JSON_KEY_LEXER