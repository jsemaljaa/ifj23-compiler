/*
 * dbg.h
 *
 * @brief Implementation of debug and testing instruments
 * @author Alina Vinogradova <xvinog00@vutbr.cz>
 */

#include "dbg.h"

// Macros area
#define PRINT_L() printf("* * * * * * * * * *\n")
#define PRINT_CHAR(c) printf("Char: %c\n", (c))
#define PRINT_TOKEN_TYPE(type) printf("Token type: %s\n", (d_translate_type(type)))
#define PRINT_STATE(state) printf("State: %s\n", (d_translate_state(state)))

const char* d_translate_type(token_type_t type){
    switch (type) {
        case TYPE_EOF: return "EOF";
        case TYPE_EOL: return "EOL";
        case TYPE_COLON: return "COLON";
        case TYPE_DOT: return "DOT";
        case TYPE_COMMA: return "COMMA";
        case TYPE_UNDERSCORE: return "UNDERSCORE";
        case TYPE_LBRACKET: return "LBRACKET";
        case TYPE_RBRACKET: return "RBRACKET";
        case TYPE_LPAR: return "LPAR";
        case TYPE_RPAR: return "RPAR";
        case TYPE_ARROW: return "ARROW";
        case TYPE_MUL: return "MUL";
        case TYPE_DIV: return "DIV";
        case TYPE_MINUS: return "MINUS";
        case TYPE_PLUS: return "PLUS";
        case TYPE_GT: return "GT";
        case TYPE_GE: return "GE";
        case TYPE_LT: return "LT";
        case TYPE_LE: return "LE";
        case TYPE_NEQ: return "NEQ";
        case TYPE_EQ: return "EQ";
        case TYPE_ASSIGN: return "ASSIGN";
        case TYPE_EXCL: return "EXCL";
        case TYPE_QUES: return "QUES";
        case TYPE_INT: return "INT";
        case TYPE_DOUBLE: return "DOUBLE";
        case TYPE_STRING: return "STRING";
        case TYPE_ID: return "ID";
        case TYPE_KW: return "KW";
        case TYPE_DBG: return "DBG";
    }
}

const char* d_translate_state(int state){
    switch (state) {
        case STATE_START: return "STATE_START";
        case STATE_COMM_BLOCK_START: return "STATE_COMM_BLOCK_START";
        case STATE_COMM_BLOCK_END: return "STATE_COMM_BLOCK_END";
        case STATE_COMM_LINE: return "STATE_COMM_LINE";
        case STATE_QUES: return "STATE_QUES";
        case STATE_UNDERSCORE: return "STATE_UNDERSCORE";
        case STATE_ID_KW: return "STATE_ID_KW";
        case STATE_STRING_START: return "STATE_STRING_START";
        case STATE_STRING_END: return "STATE_STRING_END";
        case STATE_STRING_MULTILINE: return "STATE_STRING_MULTILINE";
        case STATE_STRING_ESCAPE: return "STATE_STRING_ESCAPE";
        case STATE_STRING_SEQ: return "STATE_STRING_SEQ";
        case STATE_STRING_SEQ_HEX: return "STATE_STRING_SEQ_HEX";
        case STATE_NUMBER_INTEGER: return "STATE_NUMBER_INTEGER";
        case STATE_NUMBER_DOUBLE_START: return "STATE_NUMBER_DOUBLE_START";
        case STATE_NUMBER_DOUBLE_END: return "STATE_NUMBER_DOUBLE_END";
        case STATE_NUMBER_EXP_START: return "STATE_NUMBER_EXP_START";
        case STATE_NUMBER_EXP_SIGN: return "STATE_NUMBER_EXP_SIGN";
        case STATE_NUMBER_EXP_END: return "STATE_NUMBER_EXP_END";
    }
}

void d_print(char c, token_t *token){
    PRINT_L();
    PRINT_CHAR(c);
    PRINT_TOKEN_TYPE(token->type);
    PRINT_L();
}