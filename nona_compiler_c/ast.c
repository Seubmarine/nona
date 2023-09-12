#include "lexer.h"
#include "nona.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <inttypes.h>
#include "string_interning.h"
#include "hashmap.h"
#include "ast.h"

int32_t i32_add(int32_t a, int32_t b) {
    printf("a: %i b: %i addition: %i\n", a, b, a + b);
    return (a + b);
}

int32_t i32_sub(int32_t a, int32_t b) {
    printf("a: %i b: %i addition: %i\n", a, b, a - b);
    return (a - b);
}

int32_t i32_multiply(int32_t a, int32_t b) {
    printf("a: %i b: %i multiply: %i\n", a, b, a * b);
    return (a * b);
}

int32_t i32_division(int32_t a, int32_t b) {
    printf("a: %i b: %i division: %i\n", a, b, a / b);
    return (a / b);
}

void    *expression_get_data(struct expression_info *expr, struct string_interner *si) {
    if (expr->expression_type == expression_type_literal)
    {
        return (((struct literal *)expr)->data);
    }
    else if (expr->expression_type == expression_type_operation)
    {
        struct operation *op = (struct operation *)expr;
        struct string_interner *si_tmp = si;
        return (operation_apply(op, si_tmp));
    }
    else if (expr->expression_type == expression_type_variable)
    {
        return (((struct variable *)expr)->data);
    }
    return (NULL);
}

void *operation_apply(struct operation *op, struct string_interner *si)
{
    if (op->lhs->return_type != op->rhs->return_type)
    {
        fprintf(stdout, "can't operate on two different type %s and %s\n", op->lhs->return_type, op->rhs->return_type);
        exit(EXIT_FAILURE);
    }
    op->expr_info.return_type = op->lhs->return_type;
    void *left = expression_get_data(op->lhs, si);
    void *right = expression_get_data(op->rhs, si);
    if (op->op_type == operation_type_addition)
    {
        if (op->expr_info.return_type == string_intern(si, "i32", 3))
            *(uint32_t *)op->data = i32_add(*(uint32_t *)left, *(uint32_t *)right);
        return (op->data);
    }
    else if (op->op_type == operation_type_multiply) {
        if (op->expr_info.return_type == string_intern(si, "i32", 3))
            *(uint32_t *)op->data = i32_multiply(*(uint32_t *)left, *(uint32_t *)right);
        return (op->data);
    }
    else if (op->op_type == operation_type_division) {
        if (op->expr_info.return_type == string_intern(si, "i32", 3))
            *(uint32_t *)op->data = i32_division(*(uint32_t *)left, *(uint32_t *)right);
        return (op->data);
    }
    else if (op->op_type == operation_type_substract) {
        if (op->expr_info.return_type == string_intern(si, "i32", 3))
            *(uint32_t *)op->data = i32_sub(*(uint32_t *)left, *(uint32_t *)right);
        return (op->data);
    }
    else if (op->op_type == operation_type_assign && op->lhs->expression_type == expression_type_variable)
    {
        *(uint32_t *)left = *(uint32_t *)right;
        return (left);
    }
    return (NULL);
}

struct literal *literal_init(uint32_t x, struct string_interner *si)
{
    struct literal *l = malloc(sizeof(*l) + sizeof(uint32_t));
    l->expr_info.expression_type = expression_type_literal;
    l->expr_info.return_type = string_intern(si, "i32", 3);
    *((uint32_t *)l->data) = x;
    
    return (l);
}

struct operation *operation_init(enum operation_type op_type, struct expression_info *lhs, struct expression_info *rhs, struct string_interner *si)
{
    if (lhs->return_type != rhs->return_type)
    {
        fprintf(stdout, "can't operate on two different type %s and %s\n", lhs->return_type, rhs->return_type);
        exit(EXIT_FAILURE);
        return (NULL);
    }
    struct operation *op = malloc(sizeof(*op) + sizeof(uint32_t));
    op->expr_info.return_type = string_intern(si, "i32", 3);
    op->expr_info.expression_type = expression_type_operation;
    op->lhs = lhs;
    op->rhs = rhs;
    op->op_type = op_type;
    return (op);
}

struct variable *variable_init(char *name, typeid type)
{
    struct variable *var = malloc(sizeof(var) + sizeof(uint32_t));
    var->info.return_type = type;
    var->var_name = name;
    var->info.expression_type = expression_type_variable;
    return (var);
}

size_t variable_hash(void *key_value)
{
    struct variable **var = key_value;
    size_t hashed = (size_t)(*var)->var_name;
    return (hashed);
}

bool variable_compare(void *hm, void *kv)
{
    struct variable **var_hm = hm;
    struct variable **var = kv;
    return ((*var_hm)->var_name == (*var)->var_name);
}

struct variable *variable_get(struct hashmap *hm, char *name)
{
    struct variable tmp = {.var_name = name};
    struct variable *tmp_ptr = &tmp;
    return (*(struct variable **)hashmap_get(hm, &tmp_ptr));
}

// int main(void)
// {
//     struct string_interner si = string_interner_init();
//     // struct hashmap variable = hashmap_init(sizeof(struct nona_identifier), hash_fnv_1a_nona_identifier, nona_identifier_compare, nona_identifier_free);
//     struct variable *var = variable_init(string_intern(&si, "minecraft", 9), string_intern(&si, "i32", 3));     //let minecraft
//     struct hashmap variables = hashmap_init(sizeof(var), variable_hash, variable_compare, NULL);
//     hashmap_insert(&variables, &var);

//     struct literal *lit_0 = literal_init(1, &si);                                                                //1

//     struct variable *var_ref = variable_get(&variables, string_intern(&si, "minecraft", 9));
    

//     printf("variable : %zu\n", sizeof(struct variable));
//     printf("operation : %zu\n", sizeof(struct operation));
//     printf("literal : %zu\n", sizeof(struct literal));
//     printf("expr info : %zu\n", sizeof(struct expression_info));

//     //minecraft = 1
//     struct operation *assign_to_var = operation_init(operation_type_assign, TO_EXPR(var_ref), TO_EXPR(lit_0), &si);
//     //apply the assignement
//     operation_apply(assign_to_var, &si);
    
//     //(minecraft + minecraft)
//     struct operation *var_plus_lit = operation_init(operation_type_addition, TO_EXPR(var_ref), TO_EXPR(var_ref), &si);

//     //(minecraft + minecraft) + (minecraft + minecraft)
//     struct operation *var_plus_lit_double = operation_init(operation_type_addition, TO_EXPR(var_plus_lit), TO_EXPR(var_plus_lit), &si);

//     // minecraft = ((minecraft + minecraft) + (minecraft + minecraft))
//     struct operation *add_to_var = operation_init(operation_type_assign, TO_EXPR(var_ref), TO_EXPR(var_plus_lit_double), &si);
    
//     //apply the assignement and evaluate all expression
//     operation_apply(assign_to_var, &si);
//     printf("%i\n", *(uint32_t *)var->data); // minecraft = 1

//     operation_apply(add_to_var, &si);
//     printf("%i\n", *(uint32_t *)var->data);
//     operation_apply(add_to_var, &si);
//     printf("%i\n", *(uint32_t *)var->data);
//     operation_apply(add_to_var, &si);
//     printf("%i\n", *(uint32_t *)var->data);
//     operation_apply(add_to_var, &si);
//     printf("%i\n", *(uint32_t *)var->data);
//     operation_apply(add_to_var, &si);
//     printf("%i\n", *(uint32_t *)var->data);
//     operation_apply(add_to_var, &si);
//     printf("%i\n", *(uint32_t *)var->data);

//     return 0;
// }
