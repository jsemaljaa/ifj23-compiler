/*
 * str.h
 *
 * @brief Implementation of dynamic string data structure
 * @author Alina Vinogradova <xvinog00@vutbr.cz>
 */

#include "string.h"
#include "malloc.h"
#include "str.h"

bool str_create(string_t *str, const int size){
    str->s = (char *)malloc(size * sizeof(char));

    if(str->s == NULL) return false;

    str->len = 0;
    str->allocated = size;

    return true;
}
