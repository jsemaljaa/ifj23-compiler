/*
 * Project: IFJ23 compiler
 *
 * parserhelpers.c
 *
 * @brief Implementation of helpers functions for parser
 *
 * @author Alina Vinogradova <xvinog00@vutbr.cz>
 */

#include "parserhelpers.h"


ht_item_t *find_var_in_symtables(string_t *key) {
    ht_item_t *global = NULL;

    // first try to find in global symtable
    global = symt_search(&gTable, key);

    // if param is NULL we can try to find it in local symtables
    // if param is not NULL we still should check if there's any locally defined variables that could cover a global one

    ht_item_t *local = symbstack_search(&localTables, key);

    // check if call parameter variable is defined
    // and if it is then check the id is not defined as function
    if (local == NULL) {
        // not defined in whole program
        if (global == NULL) return NULL;
    } else {
        global = local;
    }

    // id defined but it's not var type of symbol
    if (global->type != var) return NULL;

    return global;
}

int init_func_keys() {
    keysCnt = 0;

    // Init at least one
    funcKeys = malloc(sizeof(string_t));

    if (funcKeys == NULL) return OTHER_ERROR;

    return NO_ERRORS;
}

int append_func_keys(string_t key) {
    keysCnt++;
    funcKeys = realloc(funcKeys, (keysCnt + 1) * sizeof(string_t));
    if (funcKeys == NULL) return OTHER_ERROR;

    funcKeys[keysCnt - 1] = key;

    return NO_ERRORS;
}

void free_func_keys() {
    for (int i = 0; i < keysCnt; i++) {
        str_free(&funcKeys[i]);
    }

    free(funcKeys);
}

bool compare_datatypes(datatype_t required, datatype_t passed) {
    return (required.type == passed.type);
}

bool is_token_const(token_type_t type) {
    return type >= TYPE_INT && type <= TYPE_STRING;
}

int token_type_to_datatype(token_type_t type, datatype_t *datatype) {
    if (datatype == NULL) {
        return OTHER_ERROR;
    }

    if (is_token_const(type)) {
        if (type == TYPE_INT) {
            datatype->type = INTEGER_DT;
        } else if (type == TYPE_DOUBLE) {
            datatype->type = DOUBLE_DT;
        } else if (type == TYPE_STRING) {
            datatype->type = STRING_DT;
        }
        datatype->nullable = false;
        return NO_ERRORS;
    } else {
        return SYNTAX_ERROR;
    }
}

int kw_to_type(keyword_t kw, datatype_t *datatype) {
    if (datatype == NULL) {
        return OTHER_ERROR;
    }
    switch (kw) {
        case K_DOUBLE:
            datatype->type = DOUBLE_DT;
            datatype->nullable = false;
            return NO_ERRORS;
        case K_INT:
            datatype->type = INTEGER_DT;
            datatype->nullable = false;
            return NO_ERRORS;
        case K_STRING:
            datatype->type = STRING_DT;
            datatype->nullable = false;
            return NO_ERRORS;
        case K_INT_N:
            datatype->type = INTEGER_DT;
            datatype->nullable = true;
            return NO_ERRORS;
        case K_STRING_N:
            datatype->type = STRING_DT;
            datatype->nullable = true;
            return NO_ERRORS;
        case K_DOUBLE_N:
            datatype->type = DOUBLE_DT;
            datatype->nullable = true;
            return NO_ERRORS;
        case K_NIL:
            datatype->type = NIL_DT;
            datatype->nullable = true;
            return NO_ERRORS;
        default:
            return SYNTAX_ERROR;
    }
}