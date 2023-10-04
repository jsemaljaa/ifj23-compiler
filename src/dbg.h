/*
 * dbg.h
 *
 * @brief Declaration of debug and testing instruments
 * @author Alina Vinogradova <xvinog00@vutbr.cz>
 */

#ifndef PROJ_DBG_H
#define PROJ_DBG_H

#include "scanner.h"

// Those are switches to debug different modules of the project
// Set to 1 to enable debug, set to 0 to disable

#define DBG_SCANNER 1




/**
 * Translate token type from enum value to string
 * @param type Token type
 * @return String representation of a token type
 */
const char* d_translate_type(token_type_t type);

/**
 * Translate state from int to string
 * @param state State
 * @return String representation of a state
 */
const char* d_translate_state(int state);


#endif //PROJ_DBG_H
