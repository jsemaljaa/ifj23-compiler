/*
 * Project: IFJ23 compiler
 *
 * symbstack.c
 *
 * @brief Implementation of a stack of symtables
 * 
 * @author Alina Vinogradova <xvinog00@vutbr.cz>
 */

#include "symbstack.h"

void symbstack_free(ht_stack_t *symbstack) {
    for (int i = 0; i < symbstack->size; i++) {
        symbstack_pop(symbstack);
    }
    free(symbstack->head);
}

int symbstack_push(ht_stack_t *symbstack) {
    htable *tab = NULL;
    tab = realloc(symbstack->head, (symbstack->size + 1) * sizeof(htable));
    if (tab == NULL) return OTHER_ERROR;

    symbstack->head = tab;
    symt_init(&symbstack->head[symbstack->size]);
    symbstack->size++;

    return NO_ERRORS;
}

void symbstack_pop(ht_stack_t *symbstack) {
    debug("In symbstack pop: symbstack size %d", symbstack->size);
    symt_free(&symbstack->head[symbstack->size - 1]);
    symbstack->size--;
}

ht_item_t *symbstack_search(ht_stack_t *symbstack, string_t *key) {
    ht_item_t *item = NULL;
    for (int i = symbstack->size - 1; i >= 0; i--) {
        item = symt_search(&symbstack->head[i], key);
    }

    return item;
}
