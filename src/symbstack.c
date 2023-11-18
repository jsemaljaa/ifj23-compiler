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

int symbstack_push(ht_stack_t *symbstack, htable *table) {
    symbstack->head = realloc(symbstack->head, (symbstack->size + 1) * sizeof(htable));

    if (symbstack->head == NULL) {
        return OTHER_ERROR;
    }

    memcpy(&symbstack->head[symbstack->size], table, sizeof(htable));

    symbstack->size++;

    return NO_ERRORS;
}

int symbstack_pop(ht_stack_t *symbstack) {
    debug("In symbstack pop: symbstack size %d", symbstack->size);

    if (symbstack->size > 0) {
        symbstack->size--;
        symt_free(&symbstack->head[symbstack->size]);
        return NO_ERRORS;
    } else return OTHER_ERROR;
}

ht_item_t *symbstack_search(ht_stack_t *symbstack, string_t *key) {
    for (int i = symbstack->size - 1; i >= 0; i--) {
        ht_item_t *item = symt_search(&symbstack->head[i], key);
        if (item != NULL) return item;
    }

    return NULL;
}
