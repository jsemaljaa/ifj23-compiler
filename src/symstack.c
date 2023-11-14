/*
 * symstack.c
 *
 * @brief Symbols stack implementation
 *
 * @author Vinogradova Alina <xvinog00@vutbr.cz> (taken from IFJ22 school project)
 */

#include "symstack.h"

void prec_stack_init(prec_stack_t *stack) {
    stack->head = NULL;
}

void prec_stack_free(prec_stack_t *stack) {
    while(prec_stack_pop(stack));
}

bool prec_stack_is_empty(prec_stack_t *stack) {
    prec_stack_item_t *head = prec_stack_head(stack); // head should be a NONTERM
    prec_stack_item_t *term = prec_stack_first_terminal(stack); // first term should be EMPTY

    bool isNonterm = head->symb == NONTERM;
    bool isEmpty = term->symb == EMPTY;

    return isNonterm && isEmpty;
}

int prec_stack_push(prec_stack_t *stack, prec_symbs_t symb, types_t datatype) {
    prec_stack_item_t *item = (prec_stack_item_t *)malloc(sizeof(prec_stack_item_t));

    if(item == NULL) return OTHER_ERROR;

    item->symb = symb;
    item->datatype = datatype;
    item->next = stack->head;
    stack->head = item;

    return NO_ERRORS;
}

bool prec_stack_pop(prec_stack_t *stack) {
    if(stack->head != NULL){
        prec_stack_item_t *current = stack->head;
        stack->head = current->next;
        free(current);
        return true;
    }

    return false;
}

prec_stack_item_t *prec_stack_head(prec_stack_t *stack) {
    return stack->head;
}

prec_stack_item_t *prec_stack_first_terminal(prec_stack_t *stack) {
    prec_stack_item_t *item;
    for(item = stack->head; item != NULL; item = item->next){
        if(item->symb < STOP) return item;
    }
    return NULL;
}