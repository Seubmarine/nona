#ifndef LEXER_H
#define LEXER_H
# include <stdlib.h>
enum token_type
{
    //Unknown
    token_unknown,

    //Keyword
    token_let,
    token_return,

    //Literal
    token_integer,

    //Separator
    token_semicolon, // ;
    token_colon, // ;
    token_eof,
    
    //Operator
    token_assignement, // =
    token_equal, // ==
    token_addition, // +

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
    token_category_unknown,
};

enum token_category get_token_category(struct token tok);
#endif