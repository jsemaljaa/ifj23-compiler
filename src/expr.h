//
// Created by Alina Vinogradova on 11/14/2023.
//

#ifndef COMPILER_EXPR_H
#define COMPILER_EXPR_H

// Precedence table
char prec_table[][16] = {
        /*                id       (        )        +        *        -        /        ??       !        ==       !=         >        >=        <         <=        %     */
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
        /* %  */{'<', '<', 'e', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', 'e'},


};

#endif //COMPILER_EXPR_H
