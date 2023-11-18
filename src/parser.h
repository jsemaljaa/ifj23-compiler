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
int save_func_call_param();
int check_call_param();



#endif // PARSER_H