#ifndef AST_H
# define AST_H
# include "inttypes.h"
# include "vector.h"
# include <stdbool.h>

typedef char * typeid;

enum operation_type {
    operation_type_addition,
    operation_type_substract,
    operation_type_assign,
    operation_type_multiply,
    operation_type_division,
    operation_type_invalid,
};

enum expression_type {
    expression_type_error,
    expression_type_literal,
    expression_type_operation,
    expression_type_variable,
    expression_type_block,
    expression_type_unit,
    expression_type_return_fn,
    expression_type_return_scope,
};

struct expression_info
{
    typeid return_type;
    enum expression_type expression_type : 4;
};

struct return_scope {
    struct expression_info expr_info;
    struct expression_info *value;
};

struct return_fn {
    struct expression_info expr_info;
    struct expression_info *value;
};


VECTOR_DEFINE(struct expression_info *, expression)

struct block {
    struct expression_info expr_info;
    vector_expression expressions;
};

struct operation {
    struct expression_info expr_info;
    enum operation_type op_type;
    struct expression_info *lhs;
    struct expression_info *rhs;
    uint8_t data[];
};

struct literal {
    struct expression_info expr_info;
    uint8_t data[];
};

struct variable {
    struct expression_info info;
    char   *var_name;
    uint8_t data[];
};

#define TO_EXPR(x) ((struct expression_info *)x)
#define TO_OPERATOR(x) ((struct operation *)x)

void *operation_apply(struct operation *op, struct string_interner *si);
struct literal *literal_init(uint32_t x, struct string_interner *si);
struct operation *operation_init(enum operation_type op_type, struct expression_info *lhs, struct expression_info *rhs, struct string_interner *si);
struct block *block_init();
bool block_is_empty(struct block *block);

struct return_fn *expression_return_init(struct expression_info *expr);
struct return_scope *return_local_init(struct expression_info *expr);
struct expression_info *unit_type_init();

void    *expression_get_data(struct expression_info *expr, struct string_interner *si);
#endif