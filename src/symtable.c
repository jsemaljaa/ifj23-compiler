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
        if (!str_create(&item->data.func->argv, STR_SIZE))
            return OTHER_ERROR;
        else return NO_ERRORS;
    } else {
        return err;
    }
}

int symt_add_var(htable *table, string_t *key) {
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
        if (!str_create(&item->data.var->attr.string, STR_SIZE) || !str_create(&item->data.var->attr.id, STR_SIZE))
            return OTHER_ERROR;
        else return NO_ERRORS;
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
                str_clear(&item->data.func->argv);
                free(item->data.func);
            } else if (item->type == var) {
                str_clear(&item->data.var->attr.string);
                str_clear(&item->data.var->attr.id);
            }
            str_clear(&item->key);
            free(item);
            item = (ht_item_t *) item->next;
        }
    }
}