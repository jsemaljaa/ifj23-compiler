/*
 * symtable.c
 *
 * @brief Symbols table implementation
 *
 * @author Vinogradova Alina <xvinog00@vutbr.cz> (2022)
 */

#include "symtable.h"


/*
 * @brief Bitwise hash algorithm (JS Hash)
 * @return hash of the string
 * @link http://www.cse.yorku.ca/~oz/hash.html
 * @cite djb2 algorithm
 */
int hash_func(string_t *key){
    int hash = 5381, i = 0;
    for(i = 0; i < key->len; ++i){
        hash = ((hash << 5) + hash) + key->s[i];
    }

    return hash;
}

void symt_init(htable *table){
    for(int i = 0; i < MAX_HT_SIZE; ++i){
        (*table)[i] = NULL;
    }
}

int symt_add_symb(htable *table, string_t *key){
    if(table == NULL) {
        return OTHER_ERROR;
    }

    unsigned int pos = hash_func(key);
    ht_item_t *new_item = (ht_item_t *)malloc(sizeof(ht_item_t));

    if(new_item == NULL) return OTHER_ERROR;

    int err = str_create(&new_item->key, STR_SIZE);
    if (err != NO_ERRORS) {
        free(new_item);
        return OTHER_ERROR;
    }

    EXEC_STR(str_copy(key, &new_item->key));
    new_item->next = (struct ht_item *) (*table)[pos % MAX_HT_SIZE];
    (*table)[pos % MAX_HT_SIZE] = new_item;

    return NO_ERRORS;
}

int symt_add_func(htable *table, string_t *key) {
    int err;
    err = symt_add_symb(table, key);
    if (err == NO_ERRORS) {
        ht_item_t *item = symt_search(table, key);
        item->type = func;
        item->data.func = malloc(sizeof(symt_func_t));
        if (item->data.func == NULL) return OTHER_ERROR;
        item->data.func->argc = 0;
        return NO_ERRORS;
    } else {
        return err;
    }
}

int symt_add_func_param(ht_item_t *item, string_t *toCall, string_t *toUse, symt_var_t var) {
    item->data.func->argc++;
    item->data.func->argv = (param_t *)realloc(item->data.func->argv, item->data.func->argc * sizeof(param_t));
    if (item->data.func->argv == NULL) {
        return OTHER_ERROR;
    }

    int currArg = item->data.func->argc - 1;
    EXEC_STR(str_create(&item->data.func->argv[currArg].callId, STR_SIZE));
    EXEC_STR(str_create(&item->data.func->argv[currArg].id, STR_SIZE));

    item->data.func->argv[currArg].attr = var;
    EXEC_STR(str_copy(toCall, &item->data.func->argv[currArg].callId));
    EXEC_STR(str_copy(toUse, &item->data.func->argv[currArg].id));

    return NO_ERRORS;
}

int symt_add_func_call_param(ht_item_t *item, string_t *callId, string_t *id, symt_var_t attr) {
    int currCall = item->data.func->callsCnt - 1;
    if (item->data.func->calls[currCall].argc == 0) {
        // if first parameter in this call then init
        item->data.func->calls[currCall].params = malloc(sizeof(param_t));
    }
    item->data.func->calls[currCall].argc++;
    int currParam = item->data.func->calls[currCall].argc - 1;

    item->data.func->calls[currCall].params = realloc(item->data.func->calls[currCall].params, currParam + 1 * sizeof(param_t));
    if (item->data.func->calls[currCall].params == NULL) return OTHER_ERROR;
    EXEC_STR(str_create(&item->data.func->calls[currCall].params[currParam].callId, STR_SIZE));
    EXEC_STR(str_create(&item->data.func->calls[currCall].params[currParam].id, STR_SIZE));
    debug("aaaaaaaaaaaaa");

    EXEC_STR(str_copy(callId, &item->data.func->calls[currCall].params[currParam].callId));
    EXEC_STR(str_copy(id, &item->data.func->calls[currCall].params[currParam].id));

    item->data.func->calls[currCall].params[currParam].attr = attr;

    return NO_ERRORS;
}

int symt_add_func_call(ht_item_t *item) {
    item->data.func->callsCnt++;
    int callsCnt = item->data.func->callsCnt;
    item->data.func->calls = (parser_func_call_t *)realloc(item->data.func->calls, callsCnt * sizeof(parser_func_call_t));
    if (item->data.func->calls == NULL) return OTHER_ERROR;

    return NO_ERRORS;
}

void symt_remove_func_call(ht_item_t *item) {
    free(&item->data.func->calls[item->data.func->callsCnt - 1]);
    item->data.func->callsCnt--;
}

int symt_zero_parameters_call(ht_item_t *item) {
    item->data.func->calls[item->data.func->callsCnt - 1].argc = 0;
    return NO_ERRORS;
}

int symt_add_var(htable *table, string_t *key, datatype_t type) {
    // Redefinice proměnné v témže bloku vede na chybu 3.
    ht_item_t *item;

    int err;
    err = symt_add_symb(table, key);
    if (err == NO_ERRORS) {
        item = symt_search(table, key);
        item->type = var;
        item->data.var = malloc(sizeof(symt_var_t));
        item->data.var->type = type;
        item->data.var->mutable = false;
        return NO_ERRORS;
    } else {
        return err;
    }
}

ht_item_t *symt_search(htable *table, string_t *key){
    if (table == NULL || key == NULL) {
        return NULL;
    }

    unsigned int pos = hash_func(key);
    ht_item_t *item = (*table)[pos % MAX_HT_SIZE];

    while (item != NULL) {
        if(!str_cmp(&item->key, key)) {
            return item;
        } else {
            item = (ht_item_t *) item->next;
        }
    }

    return NULL;
}

void symt_free(htable *table) {
    if (table == NULL)
        return;

    for (int i = 0; i < MAX_HT_SIZE; i++) {
        ht_item_t *item = (*table)[i];
        while (item != NULL) {
            ht_item_t *next_item = (ht_item_t *) item->next;

            if (item->type == func && item->data.func != NULL) {
                if (item->data.func != NULL) {
                    if (item->data.func->isDefined) {
                        // Ensure valid argc before accessing argv
                        if (item->data.func->argc > 0 && item->data.func->argv != NULL) {
                            for (int j = 0; j < item->data.func->argc; j++) {
                                str_clear(&item->data.func->argv[j].callId);
                                str_clear(&item->data.func->argv[j].id);
                            }
                            free(item->data.func->argv);
                        }
                        free(item->data.func);
                    }
                }
            }

            str_clear(&item->key);
            free(item);
            item = next_item;
        }
    }
}