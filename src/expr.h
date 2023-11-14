//
// Created by Alina Vinogradova on 11/14/2023.
//

#ifndef COMPILER_EXPR_H
#define COMPILER_EXPR_H

// Precedence table
char prec_table[][16] = {
        /*                id       (        )        +        *        -        /        ??       !        ==       !=         >        >=        <         <=        $     */
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

typedef enum {
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
    LE,     // >=
    EMPTY,
    STOP,
    NONTERM,
    ERR
} prec_symbs_t;

#endif //COMPILER_EXPR_H