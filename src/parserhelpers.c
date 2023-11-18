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

int init_builtins() {
    const char *builtins[BUILTINS_SIZE] = {
            "readString",
            "readInt",
            "readDouble",
            "write",
            "Int2Double",
            "Double2Int",
            "length",
            "substring",
            "ord",
            "chr"
    };

    const keyword_t rets[BUILTINS_SIZE] = {
            K_STRING_N,
            K_INT_N,
            K_DOUBLE_N,
            K_NONE,
            K_DOUBLE,
            K_INT,
            K_INT,
            K_STRING_N,
            K_INT,
            K_STRING
    };

    datatype_t dt;

    string_t key;
    str_create(&key, STR_SIZE);

    for (int i = 0; i < BUILTINS_SIZE; i++) {

        debug("current word: %s", builtins[i]);
        str_copy_word(builtins[i], &key);
        debug("after copy: %s", key.s);
        EXEC(symt_add_func(&gTable, &key));

        ht_item_t *item = symt_search(&gTable, &key);
        if (item == NULL) return OTHER_ERROR;

        item->type = func;
        item->data.func->isDefined = true;

        if (!strcmp(builtins[i], "write")) {
            dt.type = NONE_DT;
            dt.nullable = true;
            item->data.func->ret = dt;
        } else {
            EXEC(kw_to_type(rets[i], &dt));
            item->data.func->ret = dt;
        }

        switch(i) {
            case 3: // "write"
                // don't know how to operate with write rn
                break;
            case 4: // "Int2Double"
            EXEC(add_builtin_param(item, "_", "THISISATERM", INTEGER_DT));
                break;
            case 5: // "Double2Int"
            EXEC(add_builtin_param(item, "_", "THISISATERM", DOUBLE_DT));
                break;
            case 6: // "length"
            EXEC(add_builtin_param(item, "_", "s", STRING_DT));
                break;
            case 7: // "substring"
            EXEC(add_builtin_param(item, "of", "s", STRING_DT));
                EXEC(add_builtin_param(item, "startingAt", "i", INTEGER_DT));
                EXEC(add_builtin_param(item, "endingBefore", "j", INTEGER_DT));
                break;
            case 8: // "ord"
            EXEC(add_builtin_param(item, "_", "c", STRING_DT));
                break;
            case 9: // "chr"
            EXEC(add_builtin_param(item, "_", "i", INTEGER_DT));
                break;
            default:
                break;
        }

    }
    return NO_ERRORS;
}

int add_builtin_param(ht_item_t *item, char *toCall, char *toUse, types_t type) {
    string_t callId, useId;
    EXEC(str_create(&callId, STR_SIZE));
    EXEC(str_create(&useId, STR_SIZE));

    EXEC(str_copy_word(toCall, &callId));
    EXEC(str_copy_word(toUse, &useId));

    symt_var_t var = construct_builtin_par(type);
    EXEC(symt_add_func_param(item, &callId, &useId, var));

    return NO_ERRORS;
}

symt_var_t construct_builtin_par(types_t type) {
    symt_var_t var;

    var.type.type = type;
    var.type.nullable = false;
    var.mutable = false;

    return var;
}

int create_local_table() {
    htable table;
    symt_init(&table);

    EXEC(symbstack_push(&localTables, &table));

    return NO_ERRORS;
}

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