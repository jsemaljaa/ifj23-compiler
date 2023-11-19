/*
 * Project: IFJ23 compiler
 *
 * expr.h
 *
 * @brief Precedence analysis declarations
 *
 * @author Vinogradova Alina <xvinog00@vutbr.cz>
 */

#ifndef COMPILER_EXPR_H
#define COMPILER_EXPR_H

#include "precstack.h"
#include "symbstack.h"
#include "parser.h"

#define PUSH_STOP() \
    do { \
        EXEC(prec_stack_push(&stack, STOP, fill_none_datatype())); \
    } while(0)


#define PUSH_SYMBOL(s, d) \
    EXEC(prec_stack_push(&stack, s, d));\

#define POP_N(n)                        \
    for (int i = 0; i < n; i++) {       \
       prec_stack_pop(&stack);          \
    }                                   \

// Precedence table

typedef enum prec_rules prec_rules_t;
typedef enum prec_symbols prec_symbs_t;



// a ?? b
// if a is nil then b
// if a is not nil then a



// Rules for analysis.


// Converts datatypes of tokens to datatypes in structure prec_datatypes_t.
int get_data_type(datatype_t *datatype);

// Finds appropriate rule to reduce the expression.
int get_rule(prec_rules_t *rule, int symbsCnt);

// Reduces operands and operators according to the rule
int reduce_operation();

// Function collecting all the necessary information to reduce the expression: number of operands and rule.
int reduce();

// Converts tokens to symbols of precedence table with prec_symbs_t datatype.
int get_symbol(prec_symbs_t *symb);

// The main function, where precedence rules must be found.
int parse_expression(int origin);

int analyze_symbol();

int shift();

bool end_of_expression();
bool is_token_allowed();
bool is_symbol_operator(prec_symbs_t symbol);
int types_are_equal(datatype_t first, datatype_t second);
int compatibility(prec_rules_t rule, prec_stack_item_t *first, prec_stack_item_t *second);
datatype_t determine_result_type(prec_rules_t rule, prec_stack_item_t *first, prec_stack_item_t *second);
datatype_t fill_none_datatype();


#endif //COMPILER_EXPR_H
