#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"
#include "ast.h"
#include "string_interning.h"
#include "hashmap.h"
#include <stdio.h>

typedef struct parser
{
    struct string_interner si;
    char *file_str;
    struct token *tokens;
    size_t length; //tokens length
    size_t index; //current token index
    struct expression_info *ast; //
    struct hashmap variables;
} parser;

parser parse_file(struct lexer_info lexing_info);
#endif