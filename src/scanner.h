/*
 * compiler.c
 *
 * @brief Declaration of data structures and functions for lexical analysis
 * @author Alina Vinogradova <xvinog00@vutbr.cz>
 */

#ifndef PROJ_SCANNER_H
#define PROJ_SCANNER_H

#include <stdio.h>
#include <stdint.h>
#include "str.h"

typedef enum {
    K_DOUBLE,
    K_ELSE,
    K_FUNC,
    K_IF,
    K_INT,
    K_LET,
    K_NIL,
    K_RETURN,
    K_STRING,
    K_VAR,
    K_WHILE
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
    TYPE_INT,           // Int
    TYPE_DOUBLE,        // Double
    TYPE_STRING,        // String
    TOKEN_ID,           // identifier
    TOKEN_KW            // keyword
} token_type_t;

typedef union token_attribute {
    string_t id;
    int integerNumber;
    double floatNumber;
    keyword_t keyword;
} token_attribute_t;

typedef struct token {
    token_type_t type;
    token_attribute_t attribute;
} token_t;

#define STATE_START 100
#define STATE_COMM_BLOCK_START 101
#define STATE_COMM_BLOCK_END 102
#define STATE_COMM_LINE 103
#define STATE_QUES 104
#define STATE_UNDERSCORE 105
#define STATE_ID 106
#define STATE_STRING_START 107
#define STATE_STRING_END 108
#define STATE_STRING_MULTILINE 109
#define STATE_STRING_ESCAPE 110
#define STATE_STRING_SEQ 111
#define STATE_STRING_SEQ_HEX 112
#define STATE_NUMBER_INTEGER 113
#define STATE_NUMBER_DOUBLE_START 114
#define STATE_NUMBER_DOUBLE_END 115
#define STATE_NUMBER_EXP_START 116
#define STATE_NUMBER_EXP_SIGN 117
#define STATE_NUMBER_EXP_END 118


#endif //PROJ_SCANNER_H
