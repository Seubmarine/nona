#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "nona.h"
#include "lexer.h"

struct keyword_compare {
    char const *identifer;
    enum token_type type;
};

#include <ctype.h>

int is_keyword(char *str) {
    size_t i = 0;
    while (!isspace(str[i]) && (isalnum(str[i]) || str[i] == '_'))
    {
        i++;
    }
    return (i);
}

int is_number(char *str)
{
    size_t i = 0;
    while (!isspace(str[i]) && (str[i] >= '0' && str[i] <= '9'))
    {
        i++;
    }
    return (i);
}

int str_to_token(char *str, size_t str_i, struct token *token) {
    while (isspace(str[str_i]))
        str_i++;
    token->span.begin = str_i;
    if (str[str_i] == '\0')
    {
        token->type = token_eof;
        token->span.begin = str_i;
        token->span.end = str_i + 1;
        return (0);
    }
    struct keyword_compare const keywords_comparator[] = {
        {"let", token_let},
        {"return", token_return},
        {";", token_semicolon},
        {":", token_colon},
        {"+", token_plus},
        {"-", token_dash},
        {"*", token_asterisk},
        {"/", token_slash},
        {"=", token_equal},
        {"(", token_rbracket_left},
        {")", token_rbracket_right },
        {"{", token_bracket_curly_left},
        {"}", token_bracket_curly_right },
    };
    size_t number_size = is_number(&str[str_i]);
    if (number_size) {
        token->type = token_integer;
        token->span.begin = str_i;
        token->span.end = str_i + number_size;
        return (1);
    }
    size_t keyword_size = is_keyword(&str[str_i]);
    if (keyword_size) //if detected a possible keyword
    {
        for (size_t i = 0; i < sizeof(keywords_comparator) / sizeof(keywords_comparator[0]); i++) {
            if (strncmp(&str[str_i], keywords_comparator[i].identifer, keyword_size) == 0) {
                token->type = keywords_comparator[i].type;
                token->span.begin = str_i;
                token->span.end = token->span.begin + keyword_size;
                return (1);
            }
        }
        token->type = token_identifier;
        token->span.begin = str_i;
        token->span.end = str_i + keyword_size;
        return (1);
    }
    for (size_t i = 0; i < sizeof(keywords_comparator) / sizeof(keywords_comparator[0]); i++) {
        if (strncmp(&str[str_i], keywords_comparator[i].identifer, strlen(keywords_comparator[i].identifer)) == 0) {
            token->type = keywords_comparator[i].type;
            token->span.begin = str_i;
            token->span.end = token->span.begin + sizeof(keywords_comparator[i].identifer) / 8;
            return (1);
        }
    }
    token->type = token_unknown;
    token->span.end = token->span.begin + 1;
    return (0);
}

enum token_category get_token_category(enum token_type token_type) {
    switch (token_type)
    {
    case token_let:
        return (token_category_keyword);
    case token_return:
        return (token_category_keyword);

    case token_integer:
        return (token_category_literal);
    
    case token_semicolon:
        return (token_category_separator);
    case token_colon:
        return (token_category_separator);
    case token_eof:
        return (token_category_separator);
    
    case token_rbracket_left:
        return (token_category_bracket);
    case token_rbracket_right:
        return (token_category_bracket);
    case token_bracket_curly_left:
        return (token_category_bracket);
    case token_bracket_curly_right:
        return (token_category_bracket);

    case token_equal:
        return (token_category_operator);
    case token_equal_equal:
        return (token_category_operator);
    case token_plus:
        return (token_category_operator);
    case token_asterisk:
        return (token_category_operator);
    case token_slash:
        return (token_category_operator);
    case token_dash:
        return (token_category_operator);

    case token_identifier:
        return (token_category_identifier);
    default:
        return (token_category_unknown);
    }
}

void token_print_debug(char *filestr, struct token tok)
{
    enum token_category category = get_token_category(tok.type);
    char const *category_str;
    switch (category)
    {
    case token_category_unknown:
        category_str = "Unknown";
        break;
    case token_category_separator:
        category_str = "Separator";
        break;
    case token_category_identifier:
        category_str = "Identifier";
        break;
    case token_category_operator:
        category_str = "Operator";
        break;
    case token_category_keyword:
        category_str = "Keyword";
        break;
    case token_category_bracket:
        category_str = "Bracket";
        break;
    case token_category_literal:
        category_str = "Literal";
        break;
    default:
        category_str = "ERROR";
        break;
    }
    fprintf(debug_stream, "%s(", category_str);
    fwrite(&filestr[tok.span.begin], sizeof(char), tok.span.end - tok.span.begin, debug_stream);
    fprintf(debug_stream, ")\n");
}

#include "string_interning.h"
struct lexer_info lexer_file(char *filestr)
{
    size_t tokens_cap = 2048;
    struct token   *tokens = calloc(tokens_cap, sizeof(*tokens));
    struct lexer_info lexing_info = {.token_length = 0, .tokens = NULL, .file_str = filestr};
    size_t token_i = 0;
    size_t filestr_i = 0;
    while (1)
    {
        if (token_i >= tokens_cap)
        {
            tokens_cap *= 2;
            tokens = realloc(tokens, sizeof(*tokens) * tokens_cap);
        }
        str_to_token(filestr, filestr_i, &tokens[token_i]);
        if (tokens[token_i].type == token_eof)
            break;
        filestr_i = tokens[token_i].span.end;
        token_i++;
    }
    for (size_t i = 0; i < token_i; i++)
    {
        token_print_debug(filestr, tokens[i]);
    }
    if (tokens[token_i].type != token_eof)
    {
        fprintf(debug_stream, "Nona: found invalid character at position: %zu\n", tokens[token_i].span.begin);
        free(tokens);
        return (lexing_info);
    }
    struct string_interner si = string_interner_init();
    size_t identifier_n = 0;
    for (size_t i = 0; i < token_i; i++)
    {
        if (tokens[i].type == token_identifier)
            identifier_n += 1;
        if (tokens[i].type == token_identifier)
        {
            struct span s = tokens[i].span;
            char *tmp = string_intern(&si, &filestr[s.begin], s.end - s.begin);
            printf("%p = %s\n", tmp, tmp);
        }
    }
    printf("token count == %zi\n", token_i);
    printf("identifier count == %zi\n", identifier_n);
    printf("string interner cap: %zu len: %zu\n", si.capacity, si.length);
    tokens = realloc(tokens, token_i);
    lexing_info.string_interner = si;
    lexing_info.token_length = token_i;
    lexing_info.tokens = tokens;
    lexing_info.file_str = filestr;
    return (lexing_info);
}
