/*
 * parser.c
 *
 * @brief Implementation of syntax and semantic analysis
 * @author Alina Vinogradova <xvinog00@vutbr.cz>
 */

#include "parser.h"


// Global and local symbols table
htable gTable, lTable;
// Current token from scanner
token_t token;
bool inBody;
int ret, code;
// Temp variable for current htable item
ht_item_t *item;
// Temp variable for token attr id
string_t tmpTokenId;

int expression(){
    // TODO
    return NO_ERRORS;
}

int var_def() {
    // TODO
    return NO_ERRORS;
}

int if_statement() {
    // TODO
    return NO_ERRORS;
}

int while_statement() {
    // TODO
    return NO_ERRORS;
}

int return_statement() {
    // TODO
    return NO_ERRORS;
}

// <statement_list> ::= <statement> <statement_list>
int statement_list() {
    if (token.type == TYPE_EOF) {
        return NO_ERRORS;
    }

    // other statements:
    // 4) <statement> ::= <expression>
    // 5) <statement> ::= <func_def>
    // 6) <statement> ::= <var_def>
    // 7) <statement> ::= <func_call>
    // 8) <statement> ::= if <expression> { <statement_list> } else { <statement_list> }
    // 9) <statement> ::= while <expression> { <statement_list> }

    switch (token.type) {
        case TYPE_ID:
            debug("<statement> ::= <expression>");
            RULE(expression()); // expression or func call
            break;
        case TYPE_KW:
            switch (token.attribute.keyword) {
                case K_FUNC:
                    debug("<statement> ::= <func_def>");
                    if (inBody) {
                        RULE(func_def());
                    } else {
                        return SYNTAX_ERROR;
                    }
                    break;
                case K_LET:
                case K_VAR:
                    debug("<statement> ::= <var_def>");
                    RULE(var_def());
                    break;
                case K_IF:
                    debug("<statement> ::= if <expression> { <statement_list> } else { <statement_list> }");
                    RULE(if_statement());
                    break;
                case K_WHILE:
                    debug("<statement> ::= while <expression> { <statement_list> }");
                    RULE(while_statement());
                    break;
                case K_RETURN:
                    debug("<return> ::= return <expression>");
                    if (inBody) {
                        return SYNTAX_ERROR;
                    } else {
                        RULE(return_statement());
                    }
                    break;
                default:
                    return SYNTAX_ERROR;
            }
            break;
        default:
            return SYNTAX_ERROR;
    }
    return NO_ERRORS;
}

int func_def() {
    debug("<func_def> ::= <func_header> <func_body>");
    // check <func_header> first
    // token here is keyword with value K_FUNC
    GET_TOKEN();
    EXPECT(token.type, TYPE_ID);
    // Každá funkce musí být definovaná, jinak končí analýza chybou 3.
    // Definice funkce nemusí lexikálně předcházet kódu pro použití této funkce, tzv. volání funkce.

    // TODO: not sure but right now redefinition of a function is SEMANTIC_DEF_ERROR
    item = symt_search(&gTable, &token.attribute.id);
    if (item == NULL) {
        code = symt_add_func(&gTable, &token.attribute.id);
        item = symt_search(&gTable, &token.attribute.id);
        EXPECT_ERROR(code);
        GET_TOKEN();
        EXPECT(token.type, TYPE_LPAR);
        RULE(parameters_list());

        GET_TOKEN();
        EXPECT(token.type, TYPE_LBRACKET);
    } else {
        if (item->type == func) return SEMANTIC_DEF_ERROR;
    }

    return NO_ERRORS;
}

// <parameters_list> ::= <parameter> <parameters_list_more> | ε
int parameters_list() {
    debug("Rule: parameters_list");
    symt_init(&lTable);
    GET_TOKEN();
    if (token.type == TYPE_RPAR) {
        item->data.func->argc = 0;
        return NO_ERRORS;
    } else if (token.type == TYPE_UNDERSCORE || token.type == TYPE_ID) {
        RULE(parameter());
        RULE(parameters_list_more());
        return NO_ERRORS;
    } else {
        return SYNTAX_ERROR;
    }
    return NO_ERRORS;
}

int parameter() {
    debug("Rule: parameter");
    // If we came from parameters_list_more rule
    if (token.type == TYPE_COMMA) GET_TOKEN();
    string_t empty;
    code = str_create(&empty, STR_SIZE);
    if (!code || !str_append(&empty, '_')) return OTHER_ERROR;
    // underscore first => skip toCall, only inFuncId exists
    if (token.type == TYPE_UNDERSCORE) {
        GET_TOKEN();
        EXPECT(token.type, TYPE_ID);
        str_copy(&token.attribute.id, &tmpTokenId);
        GET_TOKEN();
        EXPECT(token.type, TYPE_COLON);
        GET_TOKEN();
        EXPECT(token.type, TYPE_KW);
        int type = kw_to_type(token.attribute.keyword);
        if (type  == OTHER_ERROR) return SYNTAX_ERROR;
        code = symt_add_func_param(item, &empty, &tmpTokenId, type);
        if (code != NO_ERRORS) return code;
    }

    RULE(parameters_list_more());
    return NO_ERRORS;
}

int parameters_list_more() {
    debug("Rule: parameters_list_more");
    GET_TOKEN();
    if (token.type == TYPE_COMMA) { // <parameters_list_more> ::= , <parameter> <parameters_list_more>
        RULE(parameter());
        RULE(parameters_list_more());
    } else if (token.type == TYPE_RPAR) {
        return NO_ERRORS;
    }
    return NO_ERRORS;
}

int parse() {
    symt_init(&gTable);
    if(!str_create(&token.attribute.id, STR_SIZE) || !str_create(&tmpTokenId, STR_SIZE)) {
        return OTHER_ERROR;
    }

    // Get first token
    GET_TOKEN();

    // We are in the main body of a program
    inBody = true;
    return statement_list();
}

int kw_to_type(keyword_t kw) {
    switch (kw) {
        case K_DOUBLE:
            return DOUBLE_DT;
        case K_INT:
            return INTEGER_DT;
        case K_STRING:
            return STRING_DT;
        case K_INT_N:
            return INTEGER_N_DT;
        case K_STRING_N:
            return STRING_N_DT;
        case K_DOUBLE_N:
            return DOUBLE_N_DT;
        default:
            return OTHER_ERROR;
    }
}

//void expect_expression(token_t *token) {
//
//    if (token->type != TYPE_INT && token->type != TYPE_STRING && token->type != TYPE_DOUBLE && token->type != TYPE_ID) {
//        error("Unexpected token %s", token_type_to_string(token->type));
//        exit(2);
//    }
//    get_token(token);
//
//    while (token->type != TYPE_EOL) {
//        expect_operator(token->type);
//        get_token(token);
//
//        if (token->type != TYPE_INT && token->type != TYPE_STRING && token->type != TYPE_DOUBLE && token->type != TYPE_ID) {
//            error("Unexpected token %s", token_type_to_string(token->type));
//            exit(2);
//        }
//
//        get_token(token);
//    }
//}

/*
 * @brief Parser entry function
 */
//bool parser_parse(token_type_t endWhen, bool firstCall, token_t *token) {
//    htable *table;
//    if (firstCall)
//    {
//        symt_init(&globalTable);
//        table = &globalTable;
//    } else {
//        symt_init(&localTable);
//        table = &localTable;
//    }
//
//    get_token(token);
//
//    while (token->type != endWhen){
//        debug("Token: %s", token_type_to_string(token->type));
//
//        switch (token->type) {
//            case TYPE_KW:
//            {
//                switch (token->attribute.keyword) {
//                    case K_LET:
//                    case K_VAR:
//                        get_token(token);
//                        EXPECT(token->type, TYPE_ID);
//
//                        // redefenition check
//                        ht_item_t *item = symt_search(table, &(token->attribute.id));
//                        if (item != NULL) {
//                            exit(SEMANTIC_DEF_ERROR);
//                        }
//
//
//                        int err = symt_add_var(table, &(token->attribute.id));
//                        if (err != NO_ERRORS) exit(err);
//
//                        item = symt_search(table, &(token->attribute.id));
//                        item->type = var;
//                        if (token->attribute.keyword == K_LET) {
//                            item->data.var->mutable = false;
//                        } else {
//                            item->data.var->mutable = true;
//                        }
//
//                        get_token(token);
//                        if (token->type == TYPE_COLON) { // 25) <var_def_statement>
//                            get_token(token);
//                            EXPECT(token->type, TYPE_KW);
//                            item->data.var->type = kw_to_type(token->attribute.keyword);
//                        } else if (token->type == TYPE_ASSIGN) { // 26) <var_def_statement>
//
//                        } else {
//                            exit(SYNTAX_ERROR);
//                        }
//
//                }
//                if (token->attribute.keyword == K_LET || token->attribute.keyword == K_VAR || token->attribute.keyword == K_IF || token->attribute.keyword == K_WHILE || token->attribute.keyword == K_FUNC) {
//                    if (token->attribute.keyword == K_LET || token->attribute.keyword == K_VAR) {
//
//                        get_token(token);
//                        EXPECT(token->type, TYPE_ID);
//
//                        int err = symt_add_var(table, &(token->attribute.id));
//
//                        if (err != 0) {
//                            exit(err);
//                        }
//
//                        ht_item_t *item = symt_search(table, &(token->attribute.id));
//                        item->type = var;
//                        item->data.var->attr = token->attribute;
//                        if (token->type == TYPE_INT) {item->data.var->type = INTEGER_DT;}
//                        else if (token->type == TYPE_DOUBLE) {item->data.var->type = DOUBLE_DT;}
//                        else if (token->type == TYPE_STRING) {item->data.var->type = STRING_DT;}
//
//                        get_token(token);
//                        if (token->type == TYPE_ASSIGN || token->type == TYPE_COLON || token->type == TYPE_EOL || token->type == TYPE_EOF) {
//                            if (token->type != TYPE_EOF && token->type != TYPE_EOL) {
//                                if (token->type == TYPE_COLON) {
//                                    get_token(token);
//                                    EXPECT(token->type, TYPE_KW);
//                                    expect_data_type(token->attribute.keyword);
//                                    get_token(token);
//                                }
//                                EXPECT(token->type, TYPE_ASSIGN);
//                                get_token(token);
//                                //expect_value(token->type);
//                                //EXPECT expression TODO
//                                //EXPECT data type of expression same as in colon
//                                //add value to symtable
//                                get_token(token);
//                                expect_two(token->type, TYPE_EOF, TYPE_EOL);
//                            }
//                        } else {
//                            exit(SYNTAX_ERROR);
//                        }
//                    } else if (token->attribute.keyword == K_WHILE) {
//                        //EXPECT expression
//                        get_token(token);
//
//                        if (token->type == TYPE_LBRACKET) {
//                            EXPECT(token->type, TYPE_EOL);
//                        } else if (token->type == TYPE_EOL) {
//                            EXPECT(token->type, TYPE_LBRACKET);
//                        } else {
//                            exit(SYNTAX_ERROR);
//                        }
//                        parser_parse(TYPE_RBRACKET, false, token);
//                    } else if (token->attribute.keyword == K_IF) {
//                        //EXPECT expression
//                        get_token(token);
//
//                        if (token->type == TYPE_LBRACKET) {
//                            EXPECT(token->type, TYPE_EOL);
//                        } else if (token->type == TYPE_EOL) {
//                            EXPECT(token->type, TYPE_LBRACKET);
//                        } else {
//                            exit(SYNTAX_ERROR);
//                        }
//                        parser_parse(TYPE_RBRACKET, false, token);
//
//                        get_token(token);
//                        if(token->type == TYPE_EOL || token->type == TYPE_EOF || (token->type == TYPE_KW && token->attribute.keyword == K_ELSE)) {
//                            if (token->type == TYPE_EOL) {
//                                get_token(token);
//                            }
//                            if (token->type == TYPE_KW && token->attribute.keyword == K_ELSE) {
//                                get_token(token);
//                                expect_two(token->type, TYPE_EOL, TYPE_LBRACKET);
//                                if (token->type == TYPE_EOL) {
//                                    get_token(token);
//                                }
//                                EXPECT(token->type, TYPE_LBRACKET);
//                                parser_parse(TYPE_RBRACKET, false, token);
//                            }
//                        }
//                    } else if (token->attribute.keyword == K_FUNC) {
//                        get_token(token);
//                        EXPECT(token->type, TYPE_ID);
//                        symt_add_func(table, &(token->attribute.id));
//
//                        string_t funcName = token->attribute.id;
//
//                        ht_item_t *itemFunc = symt_search(table, &funcName);
//                        itemFunc->type = func;
//
//                        get_token(token);
//                        EXPECT(token->type, TYPE_LPAR);
//
//                        get_token(token);
//                        string_t args;
//
//                        if (str_create(&args, 30) != true) {
//                            exit(-2);
//                        };
//
//                        while (token->type != TYPE_RPAR) {
//
//                            expect_two(token->type, TYPE_ID, TYPE_UNDERSCORE);
//
//                            if (token->type == TYPE_UNDERSCORE) {
//                                get_token(token);
//                            }
//
//                            EXPECT(token->type, TYPE_ID);
//                            str_concat(&args, &(token->attribute.id));
//                            get_token(token);
//                            EXPECT(token->type, TYPE_COLON);
//                            get_token(token);
//                            expect_data_type(token->type);
//                            str_concat(&args, token->attribute.keyword);
//                            str_append(&args, ';');
//                            get_token(token);
//                            // maybe EXPECT comma?
//                        }
//
//                        get_token(token);
//                        if (token->type != TYPE_ARROW && token->type != TYPE_LBRACKET) {
//                            exit(SYNTAX_ERROR);
//                        }
//
//                        if (token->type == TYPE_ARROW) {
//                            get_token(token);
//                            expect_data_type(token);
//                            if (token->attribute.keyword == K_STRING) {itemFunc->data.func->ret = STRING_DT;}
//                            else if (token->attribute.keyword == K_INT) {itemFunc->data.func->ret = INTEGER_DT;}
//                            else if (token->attribute.keyword == K_DOUBLE) {itemFunc->data.func->ret = DOUBLE_DT;}
//                            get_token(token);
//                        } else {
//                            itemFunc->data.func->ret = NIL_DT;
//                        }
//
//                        EXPECT(token->type, TYPE_LBRACKET);
//                        get_token(token);
//                        EXPECT(token->type, TYPE_EOL);
//
//                        while (token->type != TYPE_RBRACKET) {
//                            while (token->attribute.keyword != K_RETURN) {
//                                parser_parse(TYPE_EOL, false, token);
//                                get_token(token);
//                            }
//                            if (itemFunc->data.func->ret != NIL_DT) {
//                                EXPECT(token->type, TYPE_ID);
//                                ht_item_t *returnVar;
//                                returnVar = symt_search(table, &(token->attribute.id));
//                                if (returnVar == NULL) {
//                                    returnVar = symt_search(globalTable,  &(token->attribute.id));
//                                    if (returnVar == NULL) {
//                                        exit(SYNTAX_ERROR);
//                                    }
//                                }
//                                if (itemFunc->data.func->ret != returnVar->data.var->type) {
//                                    exit(SYNTAX_ERROR);
//                                }
//                            } else {
//                                EXPECT(token->type, TYPE_EOL);
//                            }
//                        }
//                        get_token(token);
//                        EXPECT(token->type, TYPE_EOL);
//
//                    }
//                } else SYNTAX_ERROR;
//            } break;
//
//            case TYPE_ID:
//            {
//                get_token(token);
//                ht_item_t *item = symt_search(table, &(token->attribute.id));
//                if (item == NULL && !firstCall) {
//                    ht_item_t *itemGlobal = symt_search(&globalTable, &(token->attribute.id));
//                    if (itemGlobal != NULL) {
//                        get_token(token);
//                        EXPECT(token->type, TYPE_ASSIGN);
//                        //EXPECT exprssion TODO
//                        //EXPECT data type of expression same as in symtable
//                        //add value to symtable
//                        expect_two(token->type, TYPE_EOF, TYPE_EOL);
//                    } else {
//                        exit(SYNTAX_ERROR);
//                    }
//                } else if (item != NULL) {
//                    get_token(token);
//                    EXPECT(token->type, TYPE_ASSIGN);
//                    //EXPECT exprssion TODO
//                    //EXPECT data type of expression same as in symtable
//                    //add value to symtable
//                    expect_two(token->type, TYPE_EOF, TYPE_EOL);
//                } else {
//                    exit(SYNTAX_ERROR);
//                }
//            } break;
//
//            default:
//            break;
//        }
//
//        get_token(token);
//    }
//
//    if (!firstCall && token->type == TYPE_EOF && endWhen != TYPE_EOF) {
//        exit(SYNTAX_ERROR);
//    }
//
//    return 0;
//}
