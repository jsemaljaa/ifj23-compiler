/*
 * Project: IFJ23 compiler
 *
 * parser.h
 *
 * @brief Parser declarations
 *
 * @author Alina Vinogradova <xvinog00@vutbr.cz>
 */

#ifndef PARSER_H
#define PARSER_H


#include "parserhelpers.h"

#define SKIP_EOL() \
    do {           \
        while (token.type == TYPE_EOL) GET_TOKEN(); \
    } while(0); \

#define GET_TOKEN_SKIP_EOL()  \
    GET_TOKEN();             \
    if (token.type == TYPE_EOL) SKIP_EOL();\


#define NEXT_NON_EOL(current, expected) \
    do {                                \
        GET_TOKEN_SKIP_EOL();           \
        EXPECT(current, expected)       \
    } while(0);                            \

#define RULE(func)                      \
    debug("Applying rule %s", #func);   \
    code = (func);                      \
    debug("Rule <%s> returned %s", #func, get_text_code(code)); \
    EXPECT_ERROR(code);                 \


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
int call_parameters_list();
int call_parameter();
int call_parameters_list_more();
int expect_else();

int save_func_call();

int kw_to_type(keyword_t kw, datatype_t *datatype);
bool is_token_const(token_type_t type);
int init_func_keys();
int append_func_keys(string_t key);
void free_func_keys();
int token_type_to_datatype(token_type_t type, datatype_t *datatype);
bool compare_datatypes(datatype_t required, datatype_t passed);
int check_call_param();
ht_item_t *find_var_in_symtables(string_t *key);
int save_func_call_param();

#endif // PARSER_H