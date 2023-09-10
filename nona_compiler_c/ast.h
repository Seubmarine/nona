#ifndef AST_H
# define AST_H
# include "inttypes.h"

typedef char * typeid;

enum operation_type {
    operation_type_addition,
    operation_type_assign,
    operation_type_multiply,
    operation_type_division,
};

enum expression_type {
    expression_type_literal,
    expression_type_operation,
    expression_type_variable,
    expression_type_block,
};

struct expression_info
{
    typeid return_type;
    enum expression_type expression_type : 4;
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
    u_int8_t data[];
};

#define TO_EXPR(x) ((struct expression_info *)x)

void *operation_apply(struct operation *op, struct string_interner *si);
struct literal *literal_init(uint32_t x, struct string_interner *si);
struct operation *operation_init(enum operation_type op_type, struct expression_info *lhs, struct expression_info *rhs, struct string_interner *si);
void    *expression_get_data(struct expression_info *expr, struct string_interner *si);
#endif