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

    str_append(&key, 'b');
    err = symt_add_var(&table, &key);
    output("symt_add_var", err);

    it = symt_search(&table, &key);
    if (it) {
        output("symt_search var", 0);
        printf("\tItem type: %s\n", it->type == func ? "func" : "var");
    }
}
