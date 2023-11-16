/*
 * parser.h
 *
 * @brief Parser declarations
 * @author Alina Vinogradova <xvinog00@vutbr.cz>
 */

#ifndef PARSER_H
#define PARSER_H

#include "symbstack.h"
#include "scanner.h"
#include "log.h"
#include "utils.h"
#include "expr.h"

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