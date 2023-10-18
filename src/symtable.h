/*
 * symtable.h
 *
 * @brief Declaration of an interface for the symbols table
 * @author Alina Vinogradova <xvinog00@vutbr.cz> (2022)
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
    INTEGER_N_DT,       // Int?
    DOUBLE_N_DT,        // Double?
    STRING_N_DT         // String?
} types_t;

typedef struct var_attr {
    types_t type;
    token_attribute_t attr;
} symt_var_t;

typedef struct func_attr {
    types_t ret;           // dynamic string for return type
    string_t argv;          // dynamic string for parameters
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

/**
 * Initialize item type 'variable' in the table
 * @param table hashtable
 * @param key item key
 * @return NO_ERRORS if successful, otherwise error code
 */
int symt_add_var(htable *table, string_t *key);

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
