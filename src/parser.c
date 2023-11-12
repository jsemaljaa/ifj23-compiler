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
    htable *table;
    if (firstCall)
    {
        symt_init(&globalTable);
        table = &globalTable;
    } else {
        symt_init(&localTable);
        table = &localTable;
    }

    get_token(token);

    while (token->type != endWhen){
        debug("Token: %s", token_type_to_string(token->type));

        switch (token->type) {
    
            case TYPE_KW:
            {
                if (token->attribute.keyword == K_LET || token->attribute.keyword == K_VAR || token->attribute.keyword == K_IF || token->attribute.keyword == K_WHILE || token->attribute.keyword == K_FUNC) {
                    if (token->attribute.keyword == K_LET || token->attribute.keyword == K_VAR) {

                        get_token(token);
                        expect(token->type, TYPE_ID);
                        
                        int err = symt_add_var(table, &(token->attribute.id));
                        
                        if (err != 0) {
                            exit(err);
                        }

                        ht_item_t *item = symt_search(table, &(token->attribute.id));
                        item->type = var;
                        item->data.var->attr = token->attribute;
                        if (token->type == TYPE_INT) {item->data.var->type = INTEGER_DT;}
                        else if (token->type == TYPE_DOUBLE) {item->data.var->type = DOUBLE_DT;}
                        else if (token->type == TYPE_STRING) {item->data.var->type = STRING_DT;}

                        get_token(token);
                        if (token->type == TYPE_ASSIGN || token->type == TYPE_COLON || token->type == TYPE_EOL || token->type == TYPE_EOF) {
                            if (token->type != TYPE_EOF && token->type != TYPE_EOL) {
                                if (token->type == TYPE_COLON) {
                                    get_token(token);
                                    expect(token->type, TYPE_KW);
                                    expect_data_type(token->attribute.keyword);
                                    get_token(token);
                                }
                                expect(token->type, TYPE_ASSIGN);
                                get_token(token);
                                //expect_value(token->type);
                                //expect expression TODO
                                //expect data type of expression same as in colon
                                //add value to symtable
                                get_token(token);
                                expect_two(token->type, TYPE_EOF, TYPE_EOL);
                            }
                        } else {
                            exit(SYNTAX_ERROR);
                        }
                    } else if (token->attribute.keyword == K_WHILE) {
                        //expect expression
                        get_token(token);

                        if (token->type == TYPE_LBRACKET) {
                            expect(token->type, TYPE_EOL);
                        } else if (token->type == TYPE_EOL) {
                            expect(token->type, TYPE_LBRACKET);
                        } else {
                            exit(SYNTAX_ERROR);
                        }
                        parser_parse(TYPE_RBRACKET, false, token);
                    } else if (token->attribute.keyword == K_IF) {
                        //expect expression
                        get_token(token);

                        if (token->type == TYPE_LBRACKET) {
                            expect(token->type, TYPE_EOL);
                        } else if (token->type == TYPE_EOL) {
                            expect(token->type, TYPE_LBRACKET);
                        } else {
                            exit(SYNTAX_ERROR);
                        }
                        parser_parse(TYPE_RBRACKET, false, token);

                        get_token(token);
                        if(token->type == TYPE_EOL || token->type == TYPE_EOF || (token->type == TYPE_KW && token->attribute.keyword == K_ELSE)) {
                            if (token->type == TYPE_EOL) {
                                get_token(token);
                            }
                            if (token->type == TYPE_KW && token->attribute.keyword == K_ELSE) {
                                get_token(token);
                                expect_two(token->type, TYPE_EOL, TYPE_LBRACKET);
                                if (token->type == TYPE_EOL) {
                                    get_token(token);
                                }
                                expect(token->type, TYPE_LBRACKET);
                                parser_parse(TYPE_RBRACKET, false, token);
                            }
                        }
                    } else if (token->attribute.keyword == K_FUNC) {
                        get_token(token);
                        expect(token->type, TYPE_ID);
                        symt_add_func(table, &(token->attribute.id));

                        string_t funcName = token->attribute.id;

                        ht_item_t *itemFunc = symt_search(table, &funcName);
                        itemFunc->type = func;

                        get_token(token);
                        expect(token->type, TYPE_LPAR);

                        get_token(token);
                        string_t args;

                        if (str_create(&args, 30) != true) {
                            exit(-2);
                        };

                        while (token->type != TYPE_RPAR) {
                            
                            expect_two(token->type, TYPE_ID, TYPE_UNDERSCORE);

                            if (token->type == TYPE_UNDERSCORE) {
                                get_token(token);
                            }

                            expect(token->type, TYPE_ID);
                            str_concat(&args, &(token->attribute.id));
                            get_token(token);
                            expect(token->type, TYPE_COLON);
                            get_token(token);
                            expect_data_type(token->type);
                            str_concat(&args, token->attribute.keyword);
                            str_append(&args, ';');
                            get_token(token);
                            // maybe expect comma?
                        }

                        get_token(token);
                        if (token->type != TYPE_ARROW && token->type != TYPE_LBRACKET) {
                            exit(SYNTAX_ERROR);
                        }

                        if (token->type == TYPE_ARROW) {
                            get_token(token);
                            expect_data_type(token);
                            if (token->attribute.keyword == K_STRING) {itemFunc->data.func->ret = STRING_DT;}
                            else if (token->attribute.keyword == K_INT) {itemFunc->data.func->ret = INTEGER_DT;}
                            else if (token->attribute.keyword == K_DOUBLE) {itemFunc->data.func->ret = DOUBLE_DT;}
                            get_token(token);
                        } else {
                            itemFunc->data.func->ret = NIL_DT;
                        }
                        
                        expect(token->type, TYPE_LBRACKET);
                        get_token(token);
                        expect(token->type, TYPE_EOL);

                        while (token->type != TYPE_RBRACKET) {
                            while (token->attribute.keyword != K_RETURN) {
                                parser_parse(TYPE_KW, false, token);
                                get_token(token);
                            }
                            if (itemFunc->data.func->ret != NIL_DT) {
                                expect(token->type, TYPE_ID);
                                ht_item_t *returnVar;
                                returnVar = symt_search(table, &(token->attribute.id));
                                if (returnVar == NULL) {
                                    returnVar = symt_search(globalTable,  &(token->attribute.id));
                                    if (returnVar == NULL) {
                                        exit(SYNTAX_ERROR);
                                    }
                                }
                                if (itemFunc->data.func->ret != returnVar->data.var->type) {
                                    exit(SYNTAX_ERROR);
                                }
                            } else {
                                expect(token->type, TYPE_EOL);
                            }
                        }

                    }
                } else SYNTAX_ERROR;
            } break;

            case TYPE_ID:
            {
                get_token(token);
                ht_item_t *item = symt_search(table, &(token->attribute.id));
                if (item == NULL && !firstCall) {
                    ht_item_t *itemGlobal = symt_search(&globalTable, &(token->attribute.id));
                    if (itemGlobal != NULL) {
                        get_token(token);
                        expect(token->type, TYPE_ASSIGN);
                        //expect exprssion TODO
                        //expect data type of expression same as in symtable
                        //add value to symtable
                        expect_two(token->type, TYPE_EOF, TYPE_EOL);
                    } else {
                        exit(SYNTAX_ERROR);
                    }
                } else if (item != NULL) {
                    get_token(token);
                    expect(token->type, TYPE_ASSIGN);
                    //expect exprssion TODO
                    //expect data type of expression same as in symtable
                    //add value to symtable
                    expect_two(token->type, TYPE_EOF, TYPE_EOL);
                } else {
                    exit(SYNTAX_ERROR);
                }
            } break;

            default:
            break;
        }

        get_token(token);
    }

    if (!firstCall && token->type == TYPE_EOF && endWhen != TYPE_EOF) {
        exit(SYNTAX_ERROR);
    }

    return 0;
}
