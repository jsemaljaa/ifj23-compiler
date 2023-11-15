//
// Created by Alina Vinogradova on 11/14/2023.
//

#ifndef COMPILER_EXPR_H
#define COMPILER_EXPR_H

#include "symtable.h"
#include "precstack.h"
#include "symbstack.h"
#include "parser.h"

// Precedence table
char prec_table[][16] = {
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

typedef enum prec_symb {
    ID,     // id
    LPAR,   // (
    RPAR,   // )
    PLUS,   // +
    MUL,    // *
    MINUS,  // -
    DIV,    // /
    QUES,   // ??
    EXCL,   // !
    EQ,     // ==
    NEQ,    // !=
    GT,     // >
    GE,     // >=
    LT,     // <
    LE,     // <=
    EMPTY,  // stack is empty: dollar
    NONTERM,
    STOP,    // reduce sign
    ERR
} prec_symbs_t;

// Rules for analysis.
typedef enum {
    NOT_RULE,		// 0. rule doesn't exist
    ID_R,           // 1. E -> id
    BRACKETS_R, 	// 2. E -> (E)
    PLUS_R, 		// 3. E -> E + E
    MUL_R,		    // 4. E -> E * E
    MINUS_R, 	    // 5. E -> E - E
    DIV_R, 		    // 6. E -> E / E
    NILCOAL_R,      // 7. E -> E ?? E
    EXCL_R,         // 8. E -> E!
    EQ_R,     	    // 9. E -> E == E
    NEQ_R,   	    // 10. E -> E != E
    GT_R, 		    // 11. E -> E > E
    GE_R, 	        // 12. E -> E >= E
    LT_R, 		    // 13. E -> E < E
    LE_R,			// 14. E -> E <= E
} prec_rules_t;

// Converts datatypes of tokens to datatypes in structure prec_datatypes_t.
datatype_t get_data_type();

// Finds appropriate rule to reduce the expression.
prec_rules_t get_rule();

// Reduces operands and operators according to the rule
int reduce_operation();

// Function collecting all the necessary information to reduce the expression: number of operands and rule.
int start_reducing();

// Converts tokens to symbols of precedence table with prec_symbs_t datatype.
prec_symbs_t get_symbol();

// The main function, where precedence rules must be found.
int parse_expression();

#endif //COMPILER_EXPR_H
