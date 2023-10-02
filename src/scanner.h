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
    TYPE_NEQ            // !=
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
#define STATE_ID_KW 101
#define STATE_EOL   102
#define STATE_EOF   104

#endif //PROJ_SCANNER_H
