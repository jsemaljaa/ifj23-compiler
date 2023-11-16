/*
 * parser.c
 *
 * @brief Implementation of syntax and semantic analysis
 * @author Alina Vinogradova <xvinog00@vutbr.cz>
 */

#include "parser.h"


// Global and local symbols table
htable gTable;
ht_stack_t localTables;

// Current scope, 0 for global
int scope = 0;
// Current token from scanner
token_t token;
bool seenReturn = false;
int ret, code;
// Temp variable for current htable item
ht_item_t *item;
// Temp variable for token attr id
string_t tmpTokenId;

// Temp variable to check if function were defined after they were called
string_t *funcKeys;
int keysCnt;

int while_statement() {
    // TODO
    return NO_ERRORS;
}

int execute_calls() {
    return NO_ERRORS;
}

// <statement_list> ::= <statement> <statement_list>
int statement_list() {
    while (token.type == TYPE_EOL) {
        GET_TOKEN();
    }

    if (token.type == TYPE_EOF) {
        if (scope != 0) {
            // if function body never reached } token
            return SYNTAX_ERROR;
        }
        // here we also have to check if all the functions that were called were also defined

        if (keysCnt != 0) {
            EXEC(execute_calls());
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
        if (scope != 0) {
            if (item->data.func->ret.type == NONE_DT || seenReturn) {
                seenReturn = false;
                scope--;
                symbstack_pop(&localTables);
                GET_TOKEN();
                return statement_list();
            } else {
                return SEMANTIC_EXPR_ERROR;
            }
        }
    } else if (token.type == TYPE_ID) {
        RULE(expression()); // expression or func call
        return statement_list();
    } else if (token.type == TYPE_KW) {
        switch (token.attribute.keyword) {
            case K_FUNC:
                if (scope == 0) {
                    RULE(func_def());
                } else {
                    return SEMANTIC_DEF_ERROR;
                }
                break;
            case K_LET:
            case K_VAR:
                RULE(var_def());
                return statement_list();
            case K_IF:
                RULE(if_statement());
                return statement_list();
            case K_WHILE:
                RULE(while_statement());
                return statement_list();
            case K_RETURN:
                if (scope == 0) {
                    return SYNTAX_ERROR;
                } else {
                    RULE(return_statement());
                    return statement_list();
                }
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
    if (item != NULL) { // check if func is not defined, but perhaps was called previously
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

    scope++;
    EXEC(symbstack_push(&localTables));
    item->scope = scope;

    RULE(parameters_list());

    if (token.type == TYPE_ARROW) {
        GET_TOKEN();
        EXPECT(token.type, TYPE_KW);
        EXEC(kw_to_type(token.attribute.keyword, &item->data.func->ret));
        GET_TOKEN();
        EXPECT(token.type, TYPE_LBRACKET);
    } else if (token.type == TYPE_LBRACKET) {
        item->data.func->ret.type = NONE_DT;
    } else return SYNTAX_ERROR;

    RULE(func_body());

    return NO_ERRORS;
}

// <parameters_list> ::= <parameter> <parameters_list_more> | ε
int parameters_list() {
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
    EXEC(str_create(&toCall, STR_SIZE));

    // first PARAMETER_NAME
    if (token.type == TYPE_ID) {
        EXEC(str_copy(&token.attribute.id, &toCall));
    } else if (token.type == TYPE_UNDERSCORE) { // underscore first => skip toCall, only inFuncId exists
        EXEC(str_append(&toCall, '_'));
    } else {
        return SYNTAX_ERROR;
    }

    // second PARAMETER_ID
    GET_TOKEN();
    if (token.type == TYPE_UNDERSCORE) { // second is underscore
        // Při použití _ jako identifikátor parametru se tento parametr v těle funkce nepoužívá.
        // not sure how to understand this, leaving this like that rn
        str_clear(&tmpTokenId);
        EXEC(str_append(&tmpTokenId, '_'));
    } else if (token.type == TYPE_ID) { // second is ID to use inside a function
        EXEC(str_copy(&token.attribute.id, &tmpTokenId));
    } else {
        return SYNTAX_ERROR;
    }

    GET_TOKEN();
    EXPECT(token.type, TYPE_COLON);
    GET_TOKEN();
    EXPECT(token.type, TYPE_KW);
    symt_var_t tmp;
    tmp.mutable = false;
    EXEC(kw_to_type(token.attribute.keyword, &tmp.type));
    if (!str_cmp(&toCall, &tmpTokenId)) return SEMANTIC_OTHER_ERROR;
    EXEC(symt_add_func_param(item, &toCall, &tmpTokenId, tmp));
    debug("tmptokenid: %s", tmpTokenId.s);
    htable *workingTable = &localTables.head[scope - 1];
    // Add a variable to local function symtable
    ht_item_t *sVar = symt_search(workingTable, &tmpTokenId);
    if (sVar && sVar->type == var) {
        return SEMANTIC_DEF_ERROR;
    }
    EXEC(symt_add_var(workingTable, &tmpTokenId, tmp.type));
    sVar = symt_search(workingTable, &tmpTokenId);
    sVar->data.var->mutable = false;
    str_free(&toCall);
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

    ht_item_t *varItem;
    htable *workingTable;
    if (scope != 0) {
        varItem = symbstack_search(&localTables, &token.attribute.id);
        workingTable = &localTables.head[scope - 1];
    } else {
        varItem = symt_search(&gTable, &token.attribute.id);
        workingTable = &gTable;
    }

    if (varItem != NULL) {
        return SEMANTIC_DEF_ERROR;
    }

    EXEC(str_copy(&token.attribute.id, &tmpTokenId));

    GET_TOKEN();

    if (token.type == TYPE_COLON) {
        GET_TOKEN();
        EXPECT(token.type, TYPE_KW);
        EXEC(kw_to_type(token.attribute.keyword, &var.type));
        GET_TOKEN();
    }

    if (token.type == TYPE_ASSIGN) {
//        EXEC(parse_expression(0));
        seenExpr = true;
    }

    if (!seenExpr) {
        if (var.type.type == NONE_DT) {
            return SYNTAX_ERROR;
        } else if (var.type.nullable) {
            debug("Init var %s with nil", tmpTokenId.s);
        }
    }

    EXEC(symt_add_var(workingTable, &tmpTokenId, var.type));

    varItem = symt_search(workingTable, &tmpTokenId);
    varItem->data.var->mutable = var.mutable;

    return NO_ERRORS;
}

int save_func_call_more() {
    if (token.type == TYPE_COMMA) {
        GET_TOKEN();
        RULE(save_func_call_param());
    } else if (token.type == TYPE_RPAR) {
        GET_TOKEN();
        return NO_ERRORS;
    } else return SYNTAX_ERROR;

    return NO_ERRORS;
}

int save_func_call_param() {
    // save id so after we can decide if it's name or id
    symt_var_t tmp;
    datatype_t dt;
    string_t undersc;
    EXEC(str_create(&undersc, STR_SIZE));
    EXEC(str_append(&undersc, '_'));
    if (is_token_const(token.type)) {
        tmp.mutable = false;
        EXEC(token_type_to_datatype(token.type, &dt));
        tmp.type = dt;
        // if callId == NULL && id == NULL we're passing const ! ! !
        EXEC(symt_add_func_call_param(item, &undersc, &undersc, tmp));
        GET_TOKEN();
        EXEC(save_func_call_more());
    } else if (token.type == TYPE_ID) {
        EXEC(str_copy(&token.attribute.id, &tmpTokenId));
        GET_TOKEN();
        if (token.type == TYPE_COLON) {
            // if we found colon then next token is param and tmpTokenId is callId
            GET_TOKEN();
            EXPECT(token.type, TYPE_ID);
            ht_item_t *var = find_var_in_symtables(&token.attribute.id);
            if (var == NULL) return SEMANTIC_UNDEF_VAR_ERROR;
            tmp.type = var->data.var->type;
            tmp.mutable = var->data.var->mutable;
            EXEC(symt_add_func_call_param(item, &tmpTokenId, &token.attribute.id, tmp));
            GET_TOKEN();
            EXEC(save_func_call_more());
        } else if (token.type == TYPE_COMMA || token.type == TYPE_RPAR) {
            // tmpTokenId has parameter name
            ht_item_t *var = find_var_in_symtables(&tmpTokenId);
            if (var == NULL) return SEMANTIC_UNDEF_VAR_ERROR;
            tmp.type = var->data.var->type;
            tmp.mutable = var->data.var->mutable;
            EXEC(symt_add_func_call_param(item, &undersc, &tmpTokenId, tmp));
            EXEC(save_func_call_more());
        } else return SYNTAX_ERROR;
    } else return SYNTAX_ERROR;
    str_free(&undersc);

    return NO_ERRORS;
}

int save_func_call() {
    // token right now is (
    GET_TOKEN();
    EXEC(symt_add_func_call(item));
    if (token.type == TYPE_RPAR) {
        EXEC(symt_zero_parameters_call(item));
    } else if (token.type == TYPE_ID || is_token_const(token.type)) {
        EXEC(save_func_call_param());
        // ht_item_t *item, string_t *callId, symt_var_t var
        // symt_add_func_call_param
    } else {
        symt_remove_func_call(item);
        return SYNTAX_ERROR;
    }
    keysCnt++;
    EXEC(append_func_keys(item->key));
    GET_TOKEN();
    return NO_ERRORS;
}

int expression(){
    // token id here
    // save first token id
    EXEC(str_copy(&token.attribute.id, &tmpTokenId));

    GET_TOKEN();

    if (token.type == TYPE_LPAR) { // if it's ( then we are trying to call a function
        item = symt_search(&gTable, &tmpTokenId);
        if (item == NULL) { // no function id in symtable, save the call, if it won't be defined then semantic error
            EXEC(symt_add_func(&gTable, &tmpTokenId));
            item = symt_search(&gTable, &tmpTokenId);
            item->data.func->isDefined = false;
            EXEC(save_func_call());
            return NO_ERRORS;
        } else if (!item->data.func->isDefined) { // function is not defined
            EXEC(save_func_call());
            return NO_ERRORS;
        } else { // function is defined
            if (item->type == var) {
                return SEMANTIC_DEF_ERROR;
            } else {
                RULE(call_parameters_list());
                item->data.func->argPos = 0;
            }
        }
    } else {
        // EXEC(parse_expression(0));

        // TODO: check if we can use statement 5 + 5 as expression

        return SYNTAX_ERROR;
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
        }
    } else if (token.type == TYPE_ID || is_token_const(token.type)) {
        if (item->data.func->isDefined) {
            RULE(call_parameter());
            return NO_ERRORS;
        } else {

        }
    } else {
        return SYNTAX_ERROR;
    }

    return NO_ERRORS;
}

int call_parameter() {
    // token here is either TYPE_ID or const
    // first check if we're working with defined function or no
    bool defined = item->data.func->isDefined;
    if (defined) {
        int currArg = item->data.func->argPos;
        if (currArg >= item->data.func->argc) return SEMANTIC_CALL_RET_ERROR;
        if (is_token_const(token.type)) {
            // example: write("hello")
            datatype_t tmp;
            EXEC(token_type_to_datatype(token.type, &tmp));

            if (!str_cmp_const(&item->data.func->argv[currArg].callId, "_")
                || !compare_datatypes(item->data.func->argv[currArg].attr.type, tmp))
                return SEMANTIC_CALL_RET_ERROR;

        } else if (token.type == TYPE_ID) {
            if (!str_cmp_const(&item->data.func->argv[currArg].callId, "_")) {
                // if current argument was defined with _ as parameter name
                // then we can proceed with semantics control
                EXEC(check_call_param());
            } else { // if callId was defined then our parameter has to match this definition
                // save current ID, so we can check the next one
                EXEC(str_copy(&token.attribute.id, &tmpTokenId));
                if (str_cmp(&tmpTokenId, &item->data.func->argv[currArg].callId))
                    return SEMANTIC_CALL_RET_ERROR;
                GET_TOKEN();
                // return syntax error if next token is not comma when it should be according to grammar rules
                EXPECT(token.type, TYPE_COLON);
                GET_TOKEN();
                // here we can have two cases: parameter "with: ID" or parameter "with: 5"
                if (is_token_const(token.type)) {
                    // if it's const check datatype
                    datatype_t tmp;
                    EXEC(token_type_to_datatype(token.type, &tmp));
                    if (!compare_datatypes(item->data.func->argv[currArg].attr.type, tmp)){
                        return SEMANTIC_CALL_RET_ERROR;
                    }
                } else if (token.type == TYPE_ID) {
                    EXEC(check_call_param());
                } else return SYNTAX_ERROR;
            }
        }
        item->data.func->argPos++;
    }

    // get next token (if no syntax error it should be , or ) token)
    GET_TOKEN();
    RULE(call_parameters_list_more());
    return NO_ERRORS;
}

int call_parameters_list_more() {
    if (token.type == TYPE_COMMA) {
        GET_TOKEN();
        RULE(call_parameter());
    } else if (token.type == TYPE_RPAR) {
        GET_TOKEN();
        return NO_ERRORS;
    }
    return NO_ERRORS;
}

int check_call_param() {
    ht_item_t *param = find_var_in_symtables(&token.attribute.id);
    if (param == NULL) return SEMANTIC_UNDEF_VAR_ERROR;

    // now we should check datatype of a variable we're passing into function
    if (!compare_datatypes(item->data.func->argv[item->data.func->argPos].attr.type, param->data.var->type)) {
        return SEMANTIC_CALL_RET_ERROR;
    }

    return NO_ERRORS;
}

int if_statement() {
    // kw if here
    GET_TOKEN();
    if (token.type == TYPE_KW) {
        if (token.attribute.keyword != K_LET) return SYNTAX_ERROR;
        else {
            // kw let in if expr case
        }
    } else {
        GET_TOKEN();
        // check for empty expression
        if (token.type == TYPE_RBRACKET) return SYNTAX_ERROR;
        while (token.type != TYPE_RBRACKET) GET_TOKEN();

        // EXEC(parse_expression(1));
        // prec analysis should stop when there's token { found
    }
    return NO_ERRORS;
}

int parse() {
    symt_init(&gTable);
    EXEC(str_create(&token.attribute.id, STR_SIZE));
    EXEC(str_create(&tmpTokenId, STR_SIZE));

    EXEC(init_func_keys());
    // Get first token
    GET_TOKEN();

    // We are in the main body of a program
    code = statement_list();

    // Memory cleaning

    free_func_keys();
    symt_free(&gTable);
    str_clear(&token.attribute.id);
    str_clear(&tmpTokenId);
    debug("after cleaning");
    return code;
}

ht_item_t *find_var_in_symtables(string_t *key) {
    ht_item_t *global = NULL;

    // first try to find in global symtable
    global = symt_search(&gTable, key);

    // if param is NULL we can try to find it in local symtables
    // if param is not NULL we still should check if there's any locally defined variables that could cover a global one

    ht_item_t *local = symbstack_search(&localTables, key);

    // check if call parameter variable is defined
    // and if it is then check the id is not defined as function
    if (local == NULL) {
        // not defined in whole program
        if (global == NULL) return NULL;
    } else {
        global = local;
    }

    // id defined but it's not var type of symbol
    if (global->type != var) return NULL;

    return global;
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

bool compare_datatypes(datatype_t required, datatype_t passed) {
    return (required.type == passed.type);
}

bool is_token_const(token_type_t type) {
    return type >= TYPE_INT && type <= TYPE_STRING;
}

int token_type_to_datatype(token_type_t type, datatype_t *datatype) {
    if (datatype == NULL) {
        return OTHER_ERROR;
    }

    if (is_token_const(type)) {
        if (type == TYPE_INT) {
            datatype->type = INTEGER_DT;
        } else if (type == TYPE_DOUBLE) {
            datatype->type = DOUBLE_DT;
        } else if (type == TYPE_STRING) {
            datatype->type = STRING_DT;
        }
        datatype->nullable = false;
        return NO_ERRORS;
    } else {
        return SYNTAX_ERROR;
    }
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
