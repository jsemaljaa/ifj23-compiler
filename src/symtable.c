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

    if (!str_create(&new_item->key, STR_SIZE)) {
        free(new_item);
        return OTHER_ERROR;
    }

    str_copy(key, &new_item->key);
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

int symt_add_func_param(ht_item_t *item, string_t *toCall, string_t *toUse, datatype_t type) {
    item->data.func->argc++;
    item->data.func->argv = (param_t *)realloc(item->data.func->argv, item->data.func->argc * sizeof(param_t));
    if (item->data.func->argv == NULL) {
        return OTHER_ERROR;
    }

    if (!str_create(&item->data.func->argv[item->data.func->argc - 1].callId, STR_SIZE)
        || !str_create(&item->data.func->argv[item->data.func->argc - 1].callId, STR_SIZE)) {
        return OTHER_ERROR;
    }

    item->data.func->argv[item->data.func->argc - 1].type = type;
    str_copy(&item->data.func->argv[item->data.func->argc - 1].callId, toCall);
    str_copy(&item->data.func->argv[item->data.func->argc - 1].inFuncId, toUse);

    return NO_ERRORS;
}

int symt_add_func_call_param(ht_item_t *item, string_t *callId, symt_var_t var) {
//    item->data.func->callsCnt++;
    int currCall = item->data.func->callsCnt - 1;
    item->data.func->calls[currCall].argc++;
    int currParam = item->data.func->calls[currCall].argc - 1;
    item->data.func->calls[currCall].params = realloc(item->data.func->calls[currCall].params, item->data.func->calls[currCall].argc * sizeof(parser_call_parameter_t));
    if (item->data.func->calls[currCall].params == NULL) return OTHER_ERROR;

    if (!str_create(&item->data.func->calls[currCall].params[currParam].callId, STR_SIZE)) return OTHER_ERROR;

    str_copy(callId, &item->data.func->calls[currCall].params[currParam].callId);
    item->data.func->calls[currCall].params[currParam].var = var;

    return NO_ERRORS;
}

int symt_add_func_call(htable *table, string_t *key, int argc, parser_call_parameter_t *params) {
    ht_item_t *item = symt_search(table, key);
    if (item == NULL) {
        int code = symt_add_func(table, key);
        if (code != NO_ERRORS) return code;
        item = symt_search(table, key);
        item->data.func->isDefined = false;
    }
    // assume we already have func as we need in *item
    item->data.func->callsCnt++;
    item->data.func->calls = realloc(item->data.func->calls, item->data.func->callsCnt * sizeof(parser_call_parameter_t));
    if (item->data.func->calls[item->data.func->callsCnt - 1].params == NULL) return OTHER_ERROR;
    item->data.func->calls[item->data.func->callsCnt - 1].argc = argc;
    if (argc == 0) {
        item->data.func->calls[item->data.func->callsCnt - 1].params = NULL;
        return NO_ERRORS;
    }
    item->data.func->calls[item->data.func->callsCnt - 1].params = malloc(argc * sizeof(parser_call_parameter_t));
    for (int i = 0; i < argc - 1; i++) {
        item->data.func->calls[item->data.func->callsCnt - 1].params[i].var = params[i].var;
        if(!str_create(&item->data.func->calls[item->data.func->callsCnt - 1].params[i].callId, STR_SIZE)) return OTHER_ERROR;
        str_copy(&params[i].callId, &item->data.func->calls[item->data.func->callsCnt - 1].params[i].callId);
    }
    item->data.func->calls[item->data.func->callsCnt - 1].params = params;

    return NO_ERRORS;
}

int symt_add_var(htable *table, string_t *key, datatype_t type) {
    // Redefinice proměnné v témže bloku vede na chybu 3.
    ht_item_t *item = symt_search(table, key);
    if (item && item->type == var) {
        return SEMANTIC_DEF_ERROR;
    }

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

void symt_free(htable *table){
    if(table == NULL)
        return;

    for (int i = 0; i < MAX_HT_SIZE; i++) {
        ht_item_t *item = (*table)[i];
        while (item != NULL) {
            if (item->type == func) {
                for (int i = 0; i < item->data.func->argc; i++) {
                    str_clear(&item->data.func->argv[i].callId);
                    str_clear(&item->data.func->argv[i].inFuncId);
                    free(&item->data.func->argv[i]);
                }
                free(item->data.func);
            }
            str_clear(&item->key);
            free(item);
            item = (ht_item_t *) item->next;
        }
    }
}