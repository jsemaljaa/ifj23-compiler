/*
 * Project: IFJ23 compiler
 *
 * str.c
 *
 * @brief Implementation of dynamic string data structure
 *
 * @author Alina Vinogradova <xvinog00@vutbr.cz>
 */


#include "str.h"

#define _CRT_SECURE_NO_WARNINGS

bool str_realloc(string_t *str, const int len){
    char *t = realloc(str->s, len);
    if (t == NULL)
        return false;
    str->s = t;
    str->allocated = len;
    return true;
}

int str_create(string_t *str, const int size){
    if ((str->s = calloc(size, sizeof(char))) == NULL)
        return OTHER_ERROR;

    str->len = 0;
    str->allocated = size;

    return NO_ERRORS;
}

int str_append(string_t *str, const char c){
    if (str == NULL || str->s == NULL)
        return OTHER_ERROR;

    int len = str->len + 2;
    if (str->allocated < len) { // len + 2 for char and \0
        if (!str_realloc(str, len))
            return OTHER_ERROR;
    }

    str->s[str->len] = c;
    str->s[++str->len] = '\0';

    return NO_ERRORS;
}

int str_copy(string_t *src, string_t *dst){
    if (src == NULL || dst == NULL || src->s == NULL || dst->s == NULL)
        return OTHER_ERROR;

    // copy src to dst
    int len = src->len + 1;
    if (len > dst->allocated) {
        if (!str_realloc(dst, len))
            return OTHER_ERROR;
    }

    strcpy(dst->s, src->s);
    dst->len = src->len;

    return NO_ERRORS;
}

int str_concat(string_t *s1, string_t *s2){
    if (s1 == NULL || s2 == NULL || s1->s == NULL || s2->s == NULL)
        return OTHER_ERROR;

    int len = s1->len + s2->len + 1;

    if (s1->allocated < len){
        if (!str_realloc(s1, len))
            return OTHER_ERROR;
    }

    strcat(s1->s, s2->s);
    s1->len = len;

    return NO_ERRORS;
}

void str_clear(string_t *str){
    if (str == NULL || str->s == NULL)
        return;

    str->len = 0;
    str->s[0] = '\0';
}

int str_cmp(string_t *s1, string_t *s2){
    return strcmp(s1->s, s2->s);
}

int str_cmp_const(string_t *s1, const char *s2){
    return strcmp(s1->s, s2);
}

int str_copy_word(const char *src, string_t *dst) {
    if (src == NULL || dst == NULL) {
        return OTHER_ERROR;
    }

    str_clear(dst);

    int len = strlen(src);

    if (len >= dst->allocated) {
        int size = len + 1;
        char *new = realloc(dst->s, size);
        if (new == NULL) {
            return OTHER_ERROR;
        }

        dst->s = new;
        dst->allocated = size;
    }

    strncpy(dst->s, src, len);

    dst->s[len] = '\0';
    dst->len = len;

    return NO_ERRORS;
}

void str_free(string_t *str){
    if (str->s != NULL){
        free(str->s);
        str->s = NULL;
    }
}
