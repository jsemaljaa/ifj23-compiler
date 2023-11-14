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
bool inFunc;
bool seenReturn = false;
int ret, code;
// Temp variable for current htable item
ht_item_t *item;
// Temp variable for token attr id
string_t tmpTokenId;

// Temp variable to check if function were defined after they were called
string_t *funcKeys;
int keysCnt;

int if_statement() {
    // TODO
    return NO_ERRORS;
}

int while_statement() {
    // TODO
    return NO_ERRORS;
}



// <statement_list> ::= <statement> <statement_list>
int statement_list() {
    while (token.type == TYPE_EOL) {
        GET_TOKEN();
    }

    if (token.type == TYPE_EOF) {
        if (inFunc) {
            return SYNTAX_ERROR;
        }
    }

    // other statements:
    // 4) <statement> ::= <expression>
    // 5) <statement> ::= <func_def>
    // 6) <statement> ::= <var_def>
    // 7) <statement> ::= <func_call>
    // 8) <statement> ::= if <expression> { <statement_list> } else { <statement_list> }
    // 9) <statement> ::= while <expression> { <statement_list> }

    if (token.type == TYPE_RBRACKET) {
        if (inFunc) {
            if (item->data.func->ret.type == NONE_DT || seenReturn) {
                seenReturn = false;
                inFunc = false;
                return NO_ERRORS;
            } else {
                return SEMANTIC_EXPR_ERROR;
            }
        }
    } else if (token.type == TYPE_ID) {
        RULE(expression()); // expression or func call
    } else if (token.type == TYPE_KW) {
        switch (token.attribute.keyword) {
            case K_FUNC:
                if (!inFunc) {
                    RULE(func_def());
                } else {
                    return SEMANTIC_DEF_ERROR;
                }
                break;
            case K_LET:
            case K_VAR:
                RULE(var_def());
                break;
            case K_IF:
                RULE(if_statement());
                break;
            case K_WHILE:
                RULE(while_statement());
                break;
            case K_RETURN:
                if (!inFunc) {
                    return SYNTAX_ERROR;
                } else {
                    RULE(return_statement());
                }
                break;
            default:
                return SYNTAX_ERROR;
        }
    }

    return NO_ERRORS;
}

int func_def() {
    // check <func_header> first
    // token here is keyword with value K_FUNC
    GET_TOKEN();
    EXPECT(token.type, TYPE_ID);
    // Každá funkce musí být definovaná, jinak končí analýza chybou 3.
    // Definice funkce nemusí lexikálně předcházet kódu pro použití této funkce, tzv. volání funkce.

    item = symt_search(&gTable, &token.attribute.id);
    if (item != NULL) { // check if func is not defined, but was called previously
        if (item->type == var || (item->type == func && item->data.func->isDefined)) {
            return SEMANTIC_DEF_ERROR;
        }
    } else {
        EXEC(symt_add_func(&gTable, &token.attribute.id));
        item = symt_search(&gTable, &token.attribute.id);
    }

    item->data.func->isDefined = true;
    // after this fork we will have function struct in item
    GET_TOKEN();
    EXPECT(token.type, TYPE_LPAR);
    RULE(parameters_list());

    if (token.type == TYPE_ARROW) {
        GET_TOKEN();
        EXPECT(token.type, TYPE_KW);
        EXEC(kw_to_type(token.attribute.keyword, &item->data.func->ret));
        GET_TOKEN();
        EXPECT(token.type, TYPE_LBRACKET);
        RULE(func_body());
    } else if (token.type == TYPE_LBRACKET) {
        item->data.func->ret.type = NONE_DT;
        RULE(func_body());
    } else return SYNTAX_ERROR;

    return NO_ERRORS;
}

// <parameters_list> ::= <parameter> <parameters_list_more> | ε
int parameters_list() {
    symt_init(&lTable);
    GET_TOKEN();
    if (token.type == TYPE_RPAR) {
        GET_TOKEN();
        item->data.func->argc = 0;
        return NO_ERRORS;
    } else if (token.type == TYPE_UNDERSCORE || token.type == TYPE_ID) {
        RULE(parameter());
        return NO_ERRORS;
    } else {
        return SYNTAX_ERROR;
    }
}

int parameter() {
    string_t toCall;
    code = str_create(&toCall, STR_SIZE);
    if (!code) return OTHER_ERROR;
    // first PARAMETER_NAME
    if (token.type == TYPE_ID) {
        str_copy(&token.attribute.id, &toCall);
    } else if (token.type == TYPE_UNDERSCORE) { // underscore first => skip toCall, only inFuncId exists
        str_append(&toCall, '_');
    } else {
        return SYNTAX_ERROR;
    }

    // second PARAMETER_ID
    GET_TOKEN();
    if (token.type == TYPE_UNDERSCORE) { // second is underscore
        // Při použití _ jako identifikátor parametru se tento parametr v těle funkce nepoužívá.
        // not sure how to understand this, leaving this like that rn
        str_clear(&tmpTokenId);
        str_append(&tmpTokenId, '_');
    } else if (token.type == TYPE_ID) { // second is ID to use inside a function
        str_copy(&token.attribute.id, &tmpTokenId);
    } else {
        return SYNTAX_ERROR;
    }

    GET_TOKEN();
    EXPECT(token.type, TYPE_COLON);
    GET_TOKEN();
    EXPECT(token.type, TYPE_KW);
    datatype_t tmp;
    code = kw_to_type(token.attribute.keyword, &tmp);
    EXPECT_ERROR(code);
    code = symt_add_func_param(item, &toCall, &tmpTokenId, tmp);
    EXPECT_ERROR(code);

    // Add a variable to local function symtable
    code = symt_add_var(&lTable, &tmpTokenId, tmp);
    EXPECT_ERROR(code);
    GET_TOKEN();
    RULE(parameters_list_more());
    return NO_ERRORS;
}

int parameters_list_more() {
    if (token.type == TYPE_COMMA) { // <parameters_list_more> ::= , <parameter> <parameters_list_more>
        GET_TOKEN();
        RULE(parameter());
    } else if (token.type == TYPE_RPAR) {
        GET_TOKEN();
        return NO_ERRORS;
    }
    return NO_ERRORS;
}

int func_body() {
    inFunc = true;
    GET_TOKEN();
    RULE(statement_list());
    return NO_ERRORS;
}

int return_statement() {
    // token here is keyword return
    GET_TOKEN();
    if (token.type == TYPE_RBRACKET) {
        if (item->data.func->ret.type == NONE_DT) {
            return NO_ERRORS;
        }
    } // TODO other cases
    return NO_ERRORS;
}

int var_def() {
    // token here is either kw let or var
    symt_var_t var;

    bool seenExpr = false;
    var.type.type = NONE_DT;
    if (token.attribute.keyword == K_LET) {
        var.mutable = false;
    } else if (token.attribute.keyword == K_VAR) {
        var.mutable = true;
    }

    GET_TOKEN();
    EXPECT(token.type, TYPE_ID);

    htable *workingTable = inFunc ? &lTable : &gTable;

    ht_item_t *variableItem = symt_search(workingTable, &token.attribute.id);

    if (variableItem != NULL) {
        return SEMANTIC_DEF_ERROR;
    }

    str_copy(&token.attribute.id, &tmpTokenId);

    GET_TOKEN();

    if (token.type == TYPE_COLON) {
        GET_TOKEN();
        EXPECT(token.type, TYPE_KW);
        code = kw_to_type(token.attribute.keyword, &var.type);
        EXPECT_ERROR(code);
        GET_TOKEN();
    }

    if (token.type == TYPE_ASSIGN) {
        // call precedence analysis here
        seenExpr = true;
    }

    if (!seenExpr) {
        if (var.type.type == NONE_DT) {
            return SYNTAX_ERROR;
        } else if (var.type.nullable) {
            debug("Init var %s with nil", tmpTokenId.s);
        }
    }

    code = symt_add_var(workingTable, &tmpTokenId, var.type);
    EXPECT_ERROR(code);

    variableItem = symt_search(workingTable, &tmpTokenId);
    variableItem->data.var->mutable = var.mutable;

    return NO_ERRORS;
}

int expression(){
    // token id here
    // save first token id
    str_copy(&token.attribute.id, &tmpTokenId);

    GET_TOKEN();

    if (token.type == TYPE_LPAR) { // if it's ( then we are trying to call a function
        item = symt_search(&gTable, &tmpTokenId);
        if (item == NULL) { // no function id in symtable, save the call, if it won't be defined then semantic error
            code = symt_add_func(&gTable, &tmpTokenId);
            EXPECT_ERROR(code);
            item = symt_search(&gTable, &tmpTokenId);
            item->data.func->isDefined = false;
            RULE(call_parameters_list());
            return NO_ERRORS;
            GET_TOKEN();
            if (token.type == TYPE_ID) {
                str_copy(&token.attribute.id, &tmpTokenId); // possible callId saved in tmpTokenId
                GET_TOKEN();
                if (token.type == TYPE_COLON) {
                    // foo(with: param) case
                } else if (token.type == TYPE_COMMA) {

                }
                if (inFunc) {
                    item = symt_search(&lTable, &token.attribute.id);
                    if (item == NULL) {
                        item = symt_search(&gTable, &token.attribute.id);
                        if (item == NULL) return SEMANTIC_UNDEF_VAR_ERROR;
                    }
                }
            }
        } else { // function is defined
            if (item->type == var) {
                return SEMANTIC_DEF_ERROR;
            } else {
                RULE(call_parameters_list(true));
            }
        }
    }

    if (inFunc) { // if we are in function then we can find defined id in htable: var in local, func in global

    }
    return NO_ERRORS;
}

//<call_parameters_list> ::= <call_parameter> <call_parameters_list_more> | ε
int call_parameters_list() {
    GET_TOKEN();
    if (token.type == TYPE_RPAR) {
        if (item->data.func->isDefined) {
            // TODO: EXPAND FUNC CALL ! ! !
            if (item->data.func->argc == 0) {
                GET_TOKEN();
                return NO_ERRORS;
            }
            else return SEMANTIC_CALL_RET_ERROR;
        } else {
            EXEC(symt_add_func_call(item));
            EXEC(symt_zero_parameters_call(item));
            EXEC(append_func_keys(item->key));
            GET_TOKEN();
            return NO_ERRORS;
        }
    } else if (token.type == TYPE_ID || is_token_const(token.type)) {
        RULE(call_parameter());
        return NO_ERRORS;
    } else {
        return SYNTAX_ERROR;
    }
}

int call_parameter() {
    // parser_call_parameter_t *params;
    // int callArgc;
    return NO_ERRORS;
}

int parse() {
    symt_init(&gTable);
    if(!str_create(&token.attribute.id, STR_SIZE) || !str_create(&tmpTokenId, STR_SIZE)) {
        return OTHER_ERROR;
    }

    code = init_func_keys();
    EXPECT_ERROR(code);
    // Get first token
    GET_TOKEN();

    // We are in the main body of a program
    inFunc = false;
    code = statement_list();

    // Memory cleaning

    free_func_keys();
    symt_free(&gTable);
    str_clear(&token.attribute.id);
    str_clear(&tmpTokenId);
    free(item);

    debug("after cleaning");
    return code;
}

int init_func_keys() {
    keysCnt = 0;

    // Init at least one
    funcKeys = malloc(sizeof(string_t));

    if (funcKeys == NULL) return OTHER_ERROR;

    return NO_ERRORS;
}

int append_func_keys(string_t key) {
    keysCnt++;
    funcKeys = realloc(funcKeys, (keysCnt + 1) * sizeof(string_t));
    if (funcKeys == NULL) return OTHER_ERROR;

    funcKeys[keysCnt - 1] = key;

    return NO_ERRORS;
}

void free_func_keys() {
    for (int i = 0; i < keysCnt; i++) {
        str_free(&funcKeys[i]);
    }

    free(funcKeys);
}

bool is_token_const(token_type_t type) {
    return type < 3;
}

int kw_to_type(keyword_t kw, datatype_t *datatype) {
    if (datatype == NULL) {
        return OTHER_ERROR;
    }
    switch (kw) {
        case K_DOUBLE:
            datatype->type = DOUBLE_DT;
            datatype->nullable = false;
            return NO_ERRORS;
        case K_INT:
            datatype->type = INTEGER_DT;
            datatype->nullable = false;
            return NO_ERRORS;
        case K_STRING:
            datatype->type = STRING_DT;
            datatype->nullable = false;
            return NO_ERRORS;
        case K_INT_N:
            datatype->type = INTEGER_DT;
            datatype->nullable = true;
            return NO_ERRORS;
        case K_STRING_N:
            datatype->type = STRING_DT;
            datatype->nullable = true;
            return NO_ERRORS;
        case K_DOUBLE_N:
            datatype->type = DOUBLE_DT;
            datatype->nullable = true;
            return NO_ERRORS;
        case K_NIL:
            datatype->type = NIL_DT;
            datatype->nullable = false;
            return NO_ERRORS;
        default:
            return SYNTAX_ERROR;
    }
}
