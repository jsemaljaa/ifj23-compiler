/*
 * compiler.c
 *
 * @brief Main module
 * @author Alina Vinogradova <xvinog00@vutbr.cz>
 */

#include <stdio.h>
#include "str.h"

int main(int argc, const char **argv){
    // currently used for dynamic strings testing
    string_t str;
    str_create(&str, 20);
    printf("Allocated size: %d\n", str.allocated);

    str_append(&str, 'a');
    str_append(&str, 'c');
    printf("String: %s\n", str.s);

    string_t t;
    str_create(&t, 1);
    str_copy(&str, &t);
    printf("Test string: %s\n", t.s);

    str_append(&t, 'e');
    str_append(&t, 'f');
    str_concat(&str, &t);
    printf("Result string: %s\n", str.s);
}