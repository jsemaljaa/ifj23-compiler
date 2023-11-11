#include "parser.h"
#include "utils.h"
#include "symtable.h"

htable globalTable;
htable localTable;

void expect_expression(token_t *token) {
   
    if (token->type != TYPE_INT && token->type != TYPE_STRING && token->type != TYPE_DOUBLE && token->type != TYPE_ID) {
        error("Unexpected token %s", token_type_to_string(token->type));
        exit(2);
    }
    get_token(token);

    while (token->type != TYPE_EOL) {
        expect_operator(token->type);
        get_token(token);

        if (token->type != TYPE_INT && token->type != TYPE_STRING && token->type != TYPE_DOUBLE && token->type != TYPE_ID) {
            error("Unexpected token %s", token_type_to_string(token->type));
            exit(2);
        }

        get_token(token);
    }
}

/*
 * @brief Parser entry function
 */
bool parser_parse(token_type_t endWhen, bool firstCall, token_t *token) {
    if (firstCall)
    {
        symt_init(&globalTable);
    } else {
        symt_init(&localTable);
    }
    
    get_token(token);

    while (token->type != endWhen){
        debug("Token: %s", token_type_to_string(token->type));

        switch (token->type) {
    
            case TYPE_KW:
            {
                if (token->attribute.keyword == K_LET || token->attribute.keyword == K_VAR || token->attribute.keyword == K_IF || token->attribute.keyword == K_WHILE || token->attribute.keyword == K_FUNC) {
                    if (token->attribute.keyword == K_LET || token->attribute.keyword == K_VAR) {
                        htable *table;
                        if (firstCall) {
                            table = &globalTable;
                        } else {
                            table = &localTable;
                        }

                        get_token(token);
                        expect(token->type, TYPE_ID);
                        
                        int err = symt_add_var(table, &(token->attribute.id));
                        
                        if (err != 0) {
                            exit(err);
                        }

                        ht_item_t *item = symt_search(table, &(token->attribute.id));
                        item->type = var;
                        item->data.var->attr = token->attribute;
                        printf("%s", item->key.s);

                        get_token(token);
                        if (token->type == TYPE_ASSIGN) {
                            get_token(token);
                            expect_value(token->type);
                            get_token(token);
                            expect_two(token->type, TYPE_EOF, TYPE_EOL);
                        } else if (token->type == TYPE_COLON) {
                            get_token(token);
                            expect(token->type, TYPE_KW);
                            expect_data_type(token->attribute.keyword);
                        } else if (token->type == TYPE_EOL) {
                        } else {
                            exit(SYNTAX_ERROR);
                        }
                    }
                    
                }
                else SYNTAX_ERROR;
            } break;

            default:
            break;
        }

        get_token(token);
    }

    return 0;
}
