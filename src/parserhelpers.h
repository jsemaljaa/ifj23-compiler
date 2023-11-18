/*
 * Project: IFJ23 compiler
 *
 * parserhelpers.c
 *
 * @brief Declarations of parser helpers functions
 *
 * @author Alina Vinogradova <xvinog00@vutbr.cz>
 */

#ifndef COMPILER_PARSERHELPERS_H
#define COMPILER_PARSERHELPERS_H

#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include "symbstack.h"
#include "scanner.h"
#include "log.h"
#include "utils.h"
#include "expr.h"

#define BUILTINS_SIZE 10


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


// Global and local symbols table
extern htable gTable;
extern ht_stack_t localTables;

// Current scope, 0 for global
extern int scope;
extern bool inFunc;
extern bool inElse;
extern int inIf;
// Current token from scanner
extern token_t token;
extern bool seenReturn;
extern int code;
// Temp variable for current htable item
extern ht_item_t *item;
// Temp variable for token attr id
extern string_t tmpTokenId;

// Temp variable to check if function were defined after they were called
extern string_t *funcKeys;
extern int keysCnt;

int kw_to_type(keyword_t kw, datatype_t *datatype);
bool is_token_const(token_type_t type);
int init_func_keys();
int append_func_keys(string_t key);
void free_func_keys();
int token_type_to_datatype(token_type_t type, datatype_t *datatype);
bool compare_datatypes(datatype_t required, datatype_t passed);
ht_item_t *find_var_in_symtables(string_t *key);
int create_local_table();

int init_builtins();
symt_var_t construct_builtin_par(types_t type);
int add_builtin_param(ht_item_t *item, char *toCall, char *toUse, types_t type);

#endif //COMPILER_PARSERHELPERS_H
