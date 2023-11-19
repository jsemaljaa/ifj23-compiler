/*
 * Project: IFJ23 compiler
 *
 * precstack.h
 *
 * @brief Data structure and functions representing a stack of symbols
 *
 * @author Vinogradova Alina <xvinog00@vutbr.cz> (taken from IFJ22 project)
 */

#ifndef COMPILER_PRECSTACK_H
#define COMPILER_PRECSTACK_H

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

//#include "parser.h"
#include "symtable.h"

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

typedef enum prec_symbols {
    ID,     // id
    LPAR,   // (
    RPAR,   // )
    PLUS,   // +
    MUL,    // *
    MINUS,  // -
    DIV,    // /
    NILCOAL,// ??
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



typedef enum prec_rules {
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

typedef struct prec_stack_item {
    prec_symbs_t symb;
    datatype_t type;
    struct prec_stack_item *next;
} prec_stack_item_t;

typedef struct prec_stack {
    prec_stack_item_t *head;
} prec_stack_t;


/*
 * @brief Stack initialization
 * @param size - size of stack to initialize
 * @return If successful, returns a pointer to stack
*/
void prec_stack_init(prec_stack_t *stack);

/*
 * @brief Deleting the stack and all the data inside
 * @param *stack - pointer to a stack to delete
 * @return nothing
*/
void prec_stack_free(prec_stack_t *stack);

/*
 * @brief Check if stack is full
 * @param *stack - pointer to a stack
 * @return true if stack is empty, false otherwise
*/
bool prec_stack_is_empty(prec_stack_t *stack);

/*
 * @brief Push given symbol into the stack
 * @param *stack - pointer to a stack
 * @param symb - symbol to be pushed
 * @param datatype - datatype of a symb
 * @return If successful, returns true, else otherwise
*/
int prec_stack_push(prec_stack_t *stack, prec_symbs_t symb, datatype_t type);

/*
 * @brief Pop a symbol from the top of a stack
 * @param *stack - pointer to a stack
 * @return If successful, return true, else otherwise
*/
bool prec_stack_pop(prec_stack_t *stack);

/*
 * @brief Get a head of a stack
 * @param *stack - pointer to a stack
 * @return Returns pointer to an item which is a head of a stack
*/
prec_stack_item_t *prec_stack_head(prec_stack_t *stack);

/*
 * @brief Find the first terminal in the stack
 * @param *stack - pointer to a stack
 * @return If successful, returns a pointer to an item containing first terminal data
*/
prec_stack_item_t *prec_stack_first_terminal(prec_stack_t *stack);

#endif //COMPILER_PRECSTACK_H
