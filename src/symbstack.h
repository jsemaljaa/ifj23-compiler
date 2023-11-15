/*
 * symbstack.h
 *
 * @brief Declaration of a stack of symtables
 * @author Alina Vinogradova <xvinog00@vutbr.cz>
 */

#ifndef COMPILER_SYMBSTACK_H
#define COMPILER_SYMBSTACK_H

#include "symtable.h"

typedef struct tables_stack {
    htable *table;
    int size;
} ht_stack_t;

void symbstack_init(ht_stack_t *symbstack);

void symbstack_free(ht_stack_t *symbstack);

int symbstack_push(ht_stack_t *symbstack);

void symbstack_pop(ht_stack_t *symbstack);

ht_item_t *symbstack_seach(ht_stack_t *symbstack, string_t *key);

#endif //COMPILER_SYMBSTACK_H
