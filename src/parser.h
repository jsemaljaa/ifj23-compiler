/*
 * parser.h
 *
 * @brief Parser declarations
 * @author Alina Vinogradova <xvinog00@vutbr.cz>
 */

#ifndef PARSER_H
#define PARSER_H

#include "symtable.h"
#include "scanner.h"
#include "log.h"
#include "utils.h"


#define EXPECT_ERROR(code)      \
    if (code != NO_ERRORS) return code;  \

#define RULE(func) \
    code = (func);                      \
    EXPECT_ERROR(code);                 \
    debug("Rule returned %d", code);

#define GET_TOKEN()                 \
    code = get_token(&token);          \
    debug("Token: %s", token_type_to_string(token.type));                                        \
    if((code) != NO_ERRORS) return code;    \


#define EXPECT(current, expected) \
    if(current != expected) { \
        error("Unexpected token %s", token_type_to_string(current)); \
        exit(2); \
    }

//bool parser_parse(token_type_t endWhen, bool firstCall, token_t *token);

int parse();

int expression();
int func_def();
int var_def();
int if_statement();
int while_statement();
int return_statement();
int parameters_list();
int parameter();
int parameters_list_more();
int func_body();

int kw_to_type(keyword_t kw);

#endif // PARSER_H