#include "parser.h"
#include "ast.h"

void ast_traverse(struct expression_info *ast, uint32_t level ,void (*fn)(struct expression_info *, uint32_t))
{
    switch (ast->expression_type)
    {
    case expression_type_operation:
        ast_traverse(TO_OPERATOR(ast)->lhs, level + 1, fn);
        fn(ast, level);
        ast_traverse(TO_OPERATOR(ast)->rhs, level + 1, fn);
        break;
    default:
        fn(ast, level);
    }
}

void print_ast(struct expression_info *expr, uint32_t level) {
    for (size_t i = 0; i < level; i++)
    {
        printf("     ");
    }
    
    switch (expr->expression_type)
    {
    case expression_type_literal:
        struct literal *lit = (struct literal *)expr;
        printf("%i", *(uint32_t *)lit->data);
        break;
    case expression_type_operation:
        switch (TO_OPERATOR(expr)->op_type)
        {
        case operation_type_addition:
            printf("+ ");
            break;
        case operation_type_multiply:
            printf("* ");
            break;
        case operation_type_division:
            printf("/ ");
            break;
        default:
            printf("wtf ");
            break;
        } 
    default:
        break;
    }
    printf("\n");
}

int operator_precedence(enum operation_type a, enum operation_type b) {
    int op_precedence_map[] = {
        [operation_type_addition] = 4,
        [operation_type_multiply] = 3,
        [operation_type_division] = 3,
        [operation_type_assign] = 2
    };
    return (op_precedence_map[a] <= op_precedence_map[b]);
}

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
    struct expression_info *lhs = *ast;
    struct operation *op = NULL;
    parse_token(parser, &rhs);
    printf("Before:\n");
    ast_traverse(*ast, 0, print_ast);
    printf("After:\n");
    
    if (rhs->expression_type == expression_type_operation)
    {
        printf("rhs is an operation\n");
        if (operator_precedence(op_type, TO_OPERATOR(rhs)->op_type)) {
            printf("lfs is better than rhs\n");
            struct expression_info *tmp = TO_OPERATOR(rhs)->lhs;
            op = operation_init(op_type, lhs, tmp, &parser->si);
            TO_OPERATOR(rhs)->lhs = TO_EXPR(op);
            *ast = rhs;
            ast_traverse(*ast, 0, print_ast);
            return ;
        }
    }
    if (lhs->expression_type == expression_type_operation)
        printf("lhs is an operation\n");
    
    op = operation_init(op_type, *ast, rhs, &parser->si);
    *ast = (struct expression_info *)op;
    ast_traverse(*ast, 0, print_ast);
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
    ast_traverse(parser.ast, 0, print_ast);
    printf("\n");
    return (parser);
}