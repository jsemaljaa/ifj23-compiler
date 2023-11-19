/*
 * Project: IFJ23 compiler
 *
 * symtable.h
 *
 * @brief Declaration of an interface for the symbols table
 *
 * @author Alina Vinogradova <xvinog00@vutbr.cz> (taken from IFJ22 project)
 */

#ifndef COMPILER_SYMTABLE_H
#define COMPILER_SYMTABLE_H

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "scanner.h"

#define MAX_HT_SIZE 101

typedef enum types {
    INTEGER_DT,         // Int
    DOUBLE_DT,          // Double
    STRING_DT,          // String
    NIL_DT,             // nil
    ANY_DT,             // datatype representing term for builtin functions
    NONE_DT             // For functions with no return datatype
} types_t;

typedef struct datatype {
    types_t type;
    // if ? then nullable is true, otherwise false
    bool nullable;
} datatype_t;

typedef struct var_attr {
    datatype_t type;
    // with keyword var => mutable = true
    // with keyword let => mutable = false
    bool mutable;
    // defined = false => not defined with any value
    // defined = true => defined with type and value
    // if the variable is present in symtable then it was at least declared
    bool defined;
} symt_var_t;

typedef struct param {
    string_t callId;
    string_t id;
    symt_var_t attr;
} param_t;

typedef struct func_call {
    int argc;
    param_t *params;
} parser_func_call_t;

typedef struct func_attr {
    bool isDefined;
    datatype_t ret;                 // Function return type
    int argc;                       // Amount of func parameters and at the same time curr index of argv array
    param_t *argv;                  // Array of parameters
    parser_func_call_t *calls;      // Array of seen function calls
    int callsCnt;                   // Amount of seen function calls, also the size of calls array
    int argPos;                     // Position of argv we're working with
} symt_func_t;

// Hashtable item representation

typedef enum item_type {
    var,
    func
} ht_item_type_t;

typedef union item_data {
    symt_func_t *func;
    symt_var_t *var;
} ht_item_data_t;

typedef struct item {
    ht_item_type_t type;
    string_t key;
    ht_item_data_t data;
    struct ht_item *next;
    int scope;
} ht_item_t;

typedef ht_item_t *htable[MAX_HT_SIZE];

/**
 * djb2 hash algorithm
 * @param key key value
 * @return hash code of a given key
 */
int hash_func(string_t *key);

/**
 * Initialize table and set the structure values
 * @param table pointer to the table
 */
void symt_init(htable *table);

/**
 * Add an item to the table
 * @param table hashtable
 * @param key item key
 * @return NO_ERRORS if successful, otherwise error code
 */
// int symt_add_symb(htable *table, string_t *key);

/**
 * Initialize item type 'function' in the table
 * @param table hashtable
 * @param key item key
 * @return NO_ERRORS if successful, otherwise error code
 */
int symt_add_func(htable *table, string_t *key);


int symt_add_func_param(ht_item_t *item, string_t *toCall, string_t *toUse, symt_var_t var);

int symt_add_func_call_param(ht_item_t *item, string_t *callId, string_t *id, symt_var_t attr);

void symt_remove_func_call(ht_item_t *item);

/**
 *
 * @param item
 * @return
 */
int symt_add_func_call(ht_item_t *item);

/**
 *
 * @param item
 * @return
 */
int symt_zero_parameters_call(ht_item_t *item);

/**
 * Initialize item type 'variable' in the table
 * @param table hashtable
 * @param key item key
 * @return NO_ERRORS if successful, otherwise error code
 */
int symt_add_var(htable *table, string_t *key, datatype_t type);

/**
 * Search for an item in hashtable
 * @param table hashtable
 * @param key item key
 * @return Pointer to the item, NULL otherwise
 */
ht_item_t *symt_search(htable *table, string_t *key);

/**
 * Table destructor
 * @param table hashtable
 */
void symt_free(htable *table);

#endif //COMPILER_SYMTABLE_H
