//
// Created by Alina Vinogradova on 10/19/2023.
//

#include "src/symtable.h"

void output(char *s, int code) {
    printf("Testing %s\n\tCode: %d\n", s, code);
}

int main() {
    int err = 0;
    htable table;
    symt_init(&table);
    output("symt_init", err);

    string_t key;
    err = str_create(&key, STR_SIZE);
    output("str_create", err);

    err = str_append(&key, 'a');
    output("str_append", err);

    err = symt_add_func(&table, &key);
    output("symt_add_func", err);


    ht_item_t *it = symt_search(&table, &key);
    if (it) {
        output("symt_search func", 0);
        printf("\tItem type: %s\n", it->type == func ? "func" : "var");
    }

    string_t empty;
    if (!str_create(&empty, STR_SIZE) || !str_append(&empty, '_')) return OTHER_ERROR;

    types_t type = INTEGER_DT;
    err = symt_add_func_param(it, &empty, &key, type);
    output("symt_add_func_param", err);

    for (int i = 0; i < it->data.func->argc; i++) {
        printf("parameter: %d", it->data.func->argv[i].type);
    }

    return 0;

    str_append(&key, 'b');
    err = symt_add_var(&table, &key);
    output("symt_add_var", err);

    it = symt_search(&table, &key);
    if (it) {
        output("symt_search var", 0);
        printf("\tItem type: %s\n", it->type == func ? "func" : "var");
    }
}
