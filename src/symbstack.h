/*
 * symbstack.h
 *
 * @brief Declaration of a stack of symtables
 * @author Alina Vinogradova <xvinog00@vutbr.cz>
 */

#ifndef COMPILER_SYMBSTACK_H
#define COMPILER_SYMBSTACK_H

#include "symtable.h"

extern int code;

typedef struct tables_stack {
    htable *head;
    int size;
} ht_stack_t;

void symbstack_free(ht_stack_t *symbstack);

int symbstack_push(ht_stack_t *symbstack);

void symbstack_pop(ht_stack_t *symbstack);

ht_item_t *symbstack_search(ht_stack_t *symbstack, string_t *key);

#endif //COMPILER_SYMBSTACK_H
