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

void symbstack_init(ht_stack_t *stack) {
    stack->head = NULL;
}

void symbstack_free(ht_stack_t *symbstack) {
    while(symbstack_pop(symbstack));
}

int symbstack_push(ht_stack_t *symbstack) {
    ht_stack_item_t *item = (ht_stack_item_t *)malloc(sizeof(ht_stack_item_t));

    if (item == NULL) return OTHER_ERROR;

    item->table = (htable *)malloc(sizeof(htable));
    if (item->table == NULL) return OTHER_ERROR;

    symt_init(item->table);

    item->next = symbstack->head;
    symbstack->head = item;

    return NO_ERRORS;
}

bool symbstack_pop(ht_stack_t *symbstack) {
    if (symbstack->head != NULL) {
        ht_stack_item_t *current = symbstack->head;
        symbstack->head = current->next;
        free(current);
        return true;
    }
    return false;
}

ht_item_t *symbstack_search(ht_stack_t *symbstack, string_t *key) {
    ht_stack_item_t *head;
    for (head = symbstack->head; head != NULL; head = head->next) {
        ht_item_t *item = symt_search(head->table, key);
        if (item != NULL) return item;
    }
    return NULL;
}
