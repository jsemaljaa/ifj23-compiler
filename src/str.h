/*
 * Project: IFJ23 compiler
 *
 * str.h
 *
 * @brief Declaration of data structure and functions to work with dynamic string
 *
 * @author Alina Vinogradova <xvinog00@vutbr.cz>
 */

#ifndef COMPILER_STR_H
#define COMPILER_STR_H

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "error.h"

//#include "dbg.h"

#define STR_SIZE 20

typedef struct string {
    int len;            // length of the string
    int allocated;      // allocated size
    char *s;            // string itself
} string_t;

/**
 * Allocate string 'str' of the size 'size'
 * @param str Pointer to the string
 * @param size Size of the string to initialize
 * @return NO_ERRORS if success, error code otherwise
 */
int str_create(string_t *str, int size);

/**
 * Append a character to the end of a string
 * @param str Pointer to the string
 * @param c Character to append
 * @return NO_ERRORS if success, error code otherwise
 */
int str_append(string_t *str, char c);

/**
 * Copy string from 'src' to 'dst'
 * @param src Pointer to a source string
 * @param dst Pointer to a destination string
 * @return NO_ERRORS if success, error code otherwise
 */
int str_copy(string_t *src, string_t *dst);

/**
 * Concatenate string 's2' at the end of the string 's1'
 * @param s1 Pointer to the first string
 * @param s2 Pointer to the second string
 * @return NO_ERRORS if success, error code otherwise
 */
int str_concat(string_t *s1, string_t *s2);

/**
 * Clear string
 * @param str Pointer to a string
 */
void str_clear(string_t *str);

/**
 * Compare two strings
 * @param s1 Pointer to the first string
 * @param s2 Pointer to the second string
 * @return 0 if strings are equal
 *         < 0 if s1 is less than s2
 *         > 0 if s1 is greater than s2
 */
int str_cmp(string_t *s1, string_t *s2);

/**
 * Compare two strings
 * @param s1 Pointer to the first string
 * @param s2 Pointer to the second string
 * @return 0 if strings are equal
 *         < 0 if s1 is less than s2
 *         > 0 if s1 is greater than s2
 */
int str_cmp_const(string_t *s1, const char *s2);

/**
 * Free memory of a string
 * @param str Pointer to a string
 */
void str_free(string_t *str);

/**
 * Reallocate given string with the size len
 * @param str Pointer to a string
 * @param len Size to reallocate
 * @return True if success, False otherwise
 */
// bool str_realloc(string_t *str, int len);

int str_copy_word(const char *src, string_t *dst);

#endif //COMPILER_STR_H
