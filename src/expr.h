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

// Precedence table

typedef enum prec_rules prec_rules_t;
typedef enum prec_symbols prec_symbs_t;

static const char prec_table[][16] = {
        /*        id   (    )    +    *    -    /    ??   !    ==  !=    >   >=    <   <=    $    */
        /* id */{'e', 'e', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>'},
        /* (  */{'<', '<', '=', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', 'e' },
        /* )  */{'<', 'e', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>'},
        /* +  */{'<', '<', '>', '>', '<', '>', '<', '>', '<', '>', '>', '>', '>', '>', '>', '>'},
        /* *  */{'<', '<', '>', '>', '>', '<', '>', '>', '<', '>', '>', '>', '>', '>', '>', '>'},
        /* -  */{'<', '<', '>', '>', '<', '>', '<', '>', '<', '>', '>', '>', '>', '>', '>', '>'},
        /* /  */{'<', '<', '>', '>', '>', '<', '>', '>', '<', '>', '>', '>', '>', '>', '>', '>'},
        /* ?? */{'<', '<', '>', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '>'},
        /* !  */{'<', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>'},
        /* == */{'<', '<', '>', '<', '<', '<', '<', '>', '<', '>', '>', '>', '>', '>', '>', '>'},
        /* != */{'<', '<', '>', '<', '<', '<', '<', '>', '<', '>', '>', '>', '>', '>', '>', '>'},
        /* >  */{'<', '<', '>', '<', '<', '<', '<', '>', '<', '>', '>', '>', '>', '>', '>', '>'},
        /* >= */{'<', '<', '>', '<', '<', '<', '<', '>', '<', '>', '>', '>', '>', '>', '>', '>'},
        /* <  */{'<', '<', '>', '<', '<', '<', '<', '>', '<', '>', '>', '>', '>', '>', '>', '>'},
        /* <= */{'<', '<', '>', '<', '<', '<', '<', '>', '<', '>', '>', '>', '>', '>', '>', '>'},
        /* $  */{'<', '<', 'e', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', 'e'},
};

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

#endif //COMPILER_EXPR_H
