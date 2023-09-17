#include "parser.h"
#include "ast.h"
#include <stdbool.h>


enum operation_type operation_type_from_token(enum token_type type) {
    enum operation_type op_type = operation_type_invalid;
    switch (type)
    {
    case token_plus:
        op_type = operation_type_addition;
        break;
    case token_dash:
        op_type = operation_type_substract;
        break;
    case token_equal:
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
    return op_type;
}

struct token *previous(parser *parser) {
    return &parser->tokens[parser->index - 1];
}

struct token *peek(parser *parser) {
    return &parser->tokens[parser->index];
}

bool is_at_end(parser *parser) {
    return peek(parser)->type == token_eof;
}

struct token *advance(parser *parser) {
    if (!is_at_end(parser))
        parser->index++;
    else
        return NULL;
    return previous(parser);
}

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

bool check(parser *parser, enum token_type type) {
    if (is_at_end(parser))
        return false;
    return peek(parser)->type == type;
}

#include <stdarg.h>
bool token_match(parser *parser, size_t token_count, ...) {
    va_list args;
    
    va_start(args, token_count);
    for (size_t i = 0; i < token_count; i++)
    {
        enum token_type type = va_arg(args, enum token_type);
        if (check(parser, type)) {
            va_end(args);
            return true;
        }
    }
    va_end(args);
    return false;
}

#define MATCH(parser, ...) token_match(parser, sizeof((enum token_type[]){__VA_ARGS__}) / sizeof(enum token_type), __VA_ARGS__)


struct expression_info *expression(parser *parser);
void parse_token_literal(parser *parser, struct expression_info **ast, struct token token);

struct expression_info *parse_number(parser *parser, struct token token) {
    uint32_t x = strtoll(&parser->file_str[token.span.begin], NULL, 10);
    struct literal *lit = literal_init(x, &parser->si);
    return TO_EXPR(lit);
}

struct block *block(parser *parser);

struct expression_info *primary(parser *parser) {
    struct token *token = peek(parser);
    
    
    if (token->type == token_identifier) {
        advance(parser);
        return parse_number(parser, *token);
    }
    else if (token->type == token_bracket_curly_left) {
        return TO_EXPR(block(parser));
    }
    else if (token->type == token_rbracket_left) {
        advance(parser);
        // Detect unit type if the next direct token is a closed round bracket
        if (peek(parser)->type == token_rbracket_right) {
            advance(parser);
            return unit_type_init(&parser->si);
        }
        struct expression_info *expr = expression(parser);
        if (advance(parser)->type == token_rbracket_right)
            return expr;
    }
    return NULL;
}



struct expression_info *unary(parser *parser) {
    return primary(parser);
}

struct expression_info *factor(parser *parser) {

    struct expression_info *left = unary(parser);

    while (MATCH(parser, token_slash, token_asterisk))
    {
        struct token *operation_token = advance(parser);
        struct expression_info *right = unary(parser);

        struct operation *operation = operation_init(
            operation_type_from_token(operation_token->type),
            left,
            right,
            &parser->si
        );

        left = TO_EXPR(operation);
    }
    
    return (TO_EXPR(left));
}

struct expression_info *term(parser *parser) {
    struct expression_info *left = factor(parser);

    while (MATCH(parser, token_plus, token_dash))
    {
        struct token *operation_token = advance(parser);
        struct expression_info *right = factor(parser);

        struct operation *operation = operation_init(
            operation_type_from_token(operation_token->type),
            left,
            right,
            &parser->si
        );

        left = TO_EXPR(operation);
    }
    
    return (TO_EXPR(left));
}

struct expression_info *comparison(parser *parser) {
    return term(parser);
}

struct expression_info *equality(parser *parser) {
    return comparison(parser);
}

struct expression_info *expression(parser *parser) {
    return equality(parser);
} 

void print_ast(struct expression_info *expr, uint32_t level) {
    for (size_t i = 0; i < level; i++)
    {
        printf("     ");
    }
    
    switch (expr->expression_type)
    {
    case expression_type_literal:
        ;
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
        case operation_type_substract:
            printf("- ");
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

struct block *block(parser *parser)
{
    if (advance(parser)->type != token_bracket_curly_left)
        return NULL;

    struct block *block = block_init();

    while (!check(parser, token_bracket_curly_right))
    {
        struct expression_info *new_expr = expression(parser);
        vector_expression_push_back(&block->expressions, new_expr);

        if (peek(parser)->type != token_semicolon) {
            break ;
        }
        advance(parser);
    }
    if (advance(parser)->type != token_bracket_curly_right) {
        //SOMETHING WENT WRONG
        return NULL;
    }
    //Insert a unit_value inside of the block if the block doens't return anything
    if (previous(parser)->type == token_semicolon && \
        vector_expression_end(&block->expressions) != NULL && \
        (*vector_expression_end(&block->expressions))->expression_type != expression_type_return_fn) {
        vector_expression_push_back(&block->expressions, unit_type_init());
    }
    return block;
}

bool block_is_empty(struct block *block) {
    return block->expressions.length == 0 || (*vector_expression_end(&block->expressions))->expression_type == expression_type_unit;
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
    parser.ast = expression(&parser);
    ast_traverse(parser.ast, 0, print_ast);
    printf("\n");
    return (parser);
}