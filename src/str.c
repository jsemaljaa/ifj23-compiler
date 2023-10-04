/*
 * str.h
 *
 * @brief Implementation of dynamic string data structure
 * @author Alina Vinogradova <xvinog00@vutbr.cz>
 */


#include "str.h"

#define _CRT_SECURE_NO_WARNINGS

bool str_create(string_t *str, const int size){
    if ((str->s = calloc(size, sizeof(char))) == NULL)
        return false;

    str->len = 0;
    str->allocated = size;

    return true;
}

bool str_append(string_t *str, const char c){
    if (str == NULL || str->s == NULL)
        return false;

    int len = str->len + 2;
    if (str->allocated < len) { // len + 2 for char and \0
        if (!str_realloc(str, len))
            return false;
    }

    str->s[str->len] = c;
    str->s[++str->len] = '\0';

    return true;
}

bool str_copy(string_t *src, string_t *dst){
    if (src == NULL || dst == NULL || src->s == NULL || dst->s == NULL)
        return false;

    // copy src to dst
    int len = src->len + 1;
    if (len > dst->allocated) {
        if (!str_realloc(dst, len))
            return false;
    }

    strcpy(dst->s, src->s);
    dst->len = src->len;

    return true;
}

bool str_concat(string_t *s1, string_t *s2){
    if (s1 == NULL || s2 == NULL || s1->s == NULL || s2->s == NULL)
        return false;

    int len = s1->len + s2->len + 1;

    if (s1->allocated < len){
        if (!str_realloc(s1, len))
            return false;
    }

    strcat(s1->s, s2->s);
    s1->len = len;

    return true;
}

bool str_clear(string_t *str){
    if (str == NULL || str->s == NULL)
        return false;

    str->len = 0;
    str->s[0] = '\0';
    return true;
}

int str_cmp(string_t *s1, string_t *s2){
    return strcmp(s1->s, s2->s);
}

int str_cmp_const(string_t *s1, const char *s2){
    return strcmp(s1->s, s2);
}

void str_free(string_t *str){
    if (str->s != NULL){
        free(str->s);
        str->s = NULL;
    }
}

bool str_realloc(string_t *str, const int len){
    char *t = realloc(str->s, len);
    if (t == NULL)
        return false;
    str->s = t;
    str->allocated = len;
    return true;
}

