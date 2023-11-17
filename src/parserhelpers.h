/*
 * Project: IFJ23 compiler
 *
 * parserhelpers.c
 *
 * @brief Declarations of parser helpers functions
 *
 * @author Alina Vinogradova <xvinog00@vutbr.cz>
 */

#ifndef COMPILER_PARSERHELPERS_H
#define COMPILER_PARSERHELPERS_H

#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include "symbstack.h"
#include "scanner.h"
#include "log.h"
#include "utils.h"
#include "expr.h"

// Global and local symbols table
extern htable gTable;
extern ht_stack_t localTables;

// Current scope, 0 for global
extern int scope;
extern bool inFunc;
extern bool inElse;
extern int inIf;
// Current token from scanner
extern token_t token;
extern bool seenReturn;
extern int ret, code;
// Temp variable for current htable item
extern ht_item_t *item;
// Temp variable for token attr id
extern string_t tmpTokenId;

// Temp variable to check if function were defined after they were called
extern string_t *funcKeys;
extern int keysCnt;


#endif //COMPILER_PARSERHELPERS_H
