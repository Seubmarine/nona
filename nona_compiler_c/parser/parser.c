#include "parser.h"
#include "ast.h"

// int operator_precedence(enum token_type a, enum token_type b) {
//     return 0;
// }

void parse_token(parser *parser, struct expression_info **ast);

struct token *consume(parser *parser) {
    struct token *current = NULL;
    if (parser->index < parser->length) {
        current = &parser->tokens[parser->index];
        parser->index++;
    }
    return (current);
}

struct token *peek(parser *parser) {
    if (parser->index + 1 < parser->length)
        return (&parser->tokens[parser->index + 1]);
    return (NULL);
}

struct expression_info **ast_right_most(struct expression_info **ast) {
    struct expression_info *ast_node = *ast;
    if (ast_node->expression_type == expression_type_operation)
    {
        struct operation *op = (struct operation *)ast_node;
        return (ast_right_most(&op->rhs));
    }
    return (ast);
}

#include <stdbool.h>
void parse_token_operation(parser *parser, struct expression_info **ast, struct token token) {
    enum operation_type op_type;
    switch (token.type)
    {
    case token_addition:
        op_type = operation_type_addition;
        break;
    case token_assignement:
        op_type = operation_type_assign;
        break;
    case token_asterisk:
        op_type = operation_type_multiply;
        break;
    case token_slash:
        op_type = operation_type_division;
        break;
    default:
        printf(__FILE__ ":%i OPERATION TYPE ERROR\n", __LINE__);
        break;
    }
    
    struct expression_info *rhs = NULL;
    struct operation *op = NULL;
    if (op_type == operation_type_division || op_type == operation_type_division) {
        struct expression_info *lhs = *ast;        
        parse_token(parser, &rhs);
        *ast = TO_EXPR( operation_init(op_type, lhs, rhs, &parser->si) );
        return ;
    }
    else {
        parse_token(parser, &rhs);
        op = operation_init(op_type, *ast, rhs, &parser->si);
    }
    *ast = (struct expression_info *)op;
}

void parse_token_literal(parser *parser, struct expression_info **ast, struct token token) {
    uint32_t x = strtoll(&parser->file_str[token.span.begin], NULL, 10);
    struct literal *lit = literal_init(x, &parser->si);
    *ast = &lit->expr_info;
}
void parse_token(parser *parser, struct expression_info **ast) {
    while (1)
    {
        struct token *current = consume(parser);
        if (current == NULL || current->type == token_semicolon) {
            printf("REACH SEMICOLON\n");
            return ;
        }
        switch (get_token_category(current->type))
        {
        case token_category_operator:
            parse_token_operation(parser, ast, *current);
            break;
        case token_category_literal:
            parse_token_literal(parser, ast, *current);
            break;
        case token_category_identifier:
            parse_token_literal(parser, ast, *current);
            break;
        default:
            printf(__FILE__":%i ERROR UNKNOWN TOKEN CATEGORY\n", __LINE__);
            return ;
        }
    }
}

parser parse_file(struct lexer_info lexing_info)
{
    parser parser = {
        .index = 0,
        .tokens = lexing_info.tokens,
        .length = lexing_info.token_length,
        .file_str = lexing_info.file_str,
        .si = lexing_info.string_interner,
        .ast = NULL};
    parse_token(&parser, &parser.ast);
    return (parser);
}