/*
 * scanner.h
 *
 * @brief Declaration of data structures and functions for lexical analysis
 * @author Alina Vinogradova <xvinog00@vutbr.cz>
 */

#ifndef PROJ_SCANNER_H
#define PROJ_SCANNER_H

#include "str.h"
#include "error.h"

#include <stdio.h>
#include <stdint.h>
#include <ctype.h>

//#include "dbg.h"

#define EOL '\n'

typedef enum {
    K_DOUBLE,           // Double
    K_ELSE,             // else
    K_FUNC,             // func
    K_IF,               // if
    K_INT,              // Int
    K_LET,              // let
    K_NIL,              // nil
    K_RETURN,           // return
    K_STRING,           // String
    K_VAR,              // var
    K_WHILE,            // while
    K_INT_N,            // Int?
    K_STRING_N,         // String?
    K_DOUBLE_N,         // Double?
    K_NONE
} keyword_t;

typedef enum {
    TYPE_EOF,           // End of file
    TYPE_EOL,           // End of line
    TYPE_COLON,         // :
    TYPE_DOT,           // .
    TYPE_COMMA,         // ,
    TYPE_UNDERSCORE,    // _
    TYPE_LBRACKET,      // {
    TYPE_RBRACKET,      // }
    TYPE_LPAR,          // (
    TYPE_RPAR,          // )
    TYPE_ARROW,         // ->
    TYPE_MUL,           // *
    TYPE_DIV,           // /
    TYPE_MINUS,         // -
    TYPE_PLUS,          // +
    TYPE_GT,            // >
    TYPE_GE,            // >=
    TYPE_LT,            // <
    TYPE_LE,            // <=
    TYPE_NEQ,           // !=
    TYPE_EQ,            // ==
    TYPE_ASSIGN,        // =
    TYPE_EXCL,          // !
    TYPE_QUES,          // ?
    TYPE_NILCOAL,       // ??
    TYPE_INT,           // Int
    TYPE_DOUBLE,        // Double
    TYPE_STRING,        // String
    TYPE_ID,            // identifier
    TYPE_KW,            // keyword
    TYPE_DBG            // debug
} token_type_t;

typedef enum {
    STATE_START = 100,
    STATE_EQUALS,
    STATE_UNDERSCORE,
    STATE_MINUS,
    STATE_DIV,
    STATE_GREATER,
    STATE_LESS,
    STATE_EXCL,
    STATE_QUES,
    STATE_ID_KW,
    STATE_STRING_START,
    STATE_STRING_END,
    STATE_STRING_MULTILINE,
    STATE_STRING_ESCAPE,
    STATE_STRING_SEQ,
    STATE_STRING_SEQ_HEX,
    STATE_NUMBER_INTEGER,
    STATE_NUMBER_DOUBLE_START,
    STATE_NUMBER_DOUBLE_END,
    STATE_NUMBER_EXP_START,
    STATE_NUMBER_EXP_SIGN,
    STATE_NUMBER_EXP_END,
    STATE_COMM_LINE,
    STATE_COMM_BLOCK_START,
    STATE_COMM_BLOCK_END,
} states_t;

typedef union token_attribute {
    string_t id;
    string_t string;
    int integerNumber;
    double floatNumber;
    keyword_t keyword;
} token_attribute_t;

typedef struct token {
    token_type_t type;
    token_attribute_t attribute;
} token_t;


/**
 *
 * @param token
 * @return
 */
int get_token(token_t *token);

#endif //PROJ_SCANNER_H
