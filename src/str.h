/*
 * str.h
 *
 * @brief Declaration of data structure and functions to work with dynamic string
 */


//
// Created by Alina Vinogradova on 10/2/2023.
//

#ifndef PROJ_STR_H
#define PROJ_STR_H

typedef struct string {
    int len;            // length of the string
    int allocated;      // allocated size
    char *str;          // string itself
} string_t;

/**
 * Allocate string 'str' of the size 'size'
 * @param str Pointer to the string
 * @param size Size of the string to initialize
 * @return True if success, False otherwise
 */
bool str_create(string_t *str, const int size);

/**
 * Append a character to the end of a string
 * @param str Pointer to the string
 * @param c Character to append
 * @return True if success, False otherwise
 */
bool str_append(string_t *str, const char c);

/**
 * Copy string from 'src' to 'dst'
 * @param src Pointer to a source string
 * @param dst Pointer to a destination string
 * @return True if success, False otherwise
 */
bool str_copy(string_t *src, string_t *dst);

/**
 * Concatenate string 's2' at the end of the string 's1'
 * @param s1 Pointer to the first string
 * @param s2 Pointer to the second string
 * @return True if success, False otherwise
 */
bool str_concat(string_t *s1, string_t *s2);

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


#endif //PROJ_STR_H
