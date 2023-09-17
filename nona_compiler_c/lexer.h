#ifndef LEXER_H
#define LEXER_H
# include <stdlib.h>
# include "string_interning.h"
enum token_type
{
    //Unknown
    token_unknown,      // something not possible

    //Keyword
    token_let,          // "let"
    token_return,       // "return"

    //Literal
    token_integer,

    //Brackets
    token_rbracket_left, // (
    token_rbracket_right, // )
    token_bracket_curly_left, // {
    token_bracket_curly_right, // }

    //Separator
    token_semicolon,    // ;
    token_colon,        // :
    token_eof,          // EOF \0
    
    //Operator
    token_plus,         // +
    token_dash,         // -
    token_asterisk,     // *
    token_equal,        // =
    token_slash,        // /
    token_bang,         // !

    //Comparison
    token_equal_equal,  // ==
    token_bang_equal,   // !=

    //Identifier
    token_identifier,
};

struct span
{
    size_t begin;
    size_t end;
};

struct token
{
    struct span span;
    enum token_type type;
};

enum token_category {
    token_category_keyword,
    token_category_separator,
    token_category_identifier,
    token_category_operator,
    token_category_literal,
    token_category_bracket,
    token_category_unknown,
};

struct lexer_info
{
    struct string_interner string_interner;
    struct token *tokens;
    char *file_str;
    size_t token_length;
};

enum token_category get_token_category(enum token_type token_type);
struct lexer_info lexer_file(char *filestr);

#endif