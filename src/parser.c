/*
 * Project: IFJ23 compiler
 *
 * parser.c
 *
 * @brief Implementation of syntax and semantic analysis
 *
 * @author Alina Vinogradova <xvinog00@vutbr.cz>
 */

#include "parser.h"

// Global and local symbols table
htable gTable;
ht_stack_t localTables;

// Current scope, 0 for global
int scope = 0;
bool inFunc = false;
ht_item_t *currFunc;
bool inElse = false;

// Variables to support multiple if and while statements
int inIf = 0;
int inWhile = 0;

// Current token from scanner
token_t token;

// Variable to check if we've seen a return statement from a function or no
bool seenReturn = false;

// Return code with error in case of any, otherwise has NO_ERRORS
int code;

// Temp variable for current htable item
ht_item_t *item;
// Temp variable for token attr id
string_t tmpTokenId;

// Temp variables to check if function were defined after they were called
string_t *funcKeys;
int keysCnt;

int execute_calls() {
    // here item contains a call we have to execute and check call parameters
    int callsCnt = item->data.func->callsCnt;

    for (int i = 0; i < callsCnt; i++) {
        int callArgc = item->data.func->calls[i].argc;
        int funcArgc = item->data.func->argc;

        if (callArgc == funcArgc) {
            if (callArgc == 0) return NO_ERRORS;

            for (int j = 0; j < callArgc; j++) {
                if(str_cmp(&item->data.func->calls[i].params[j].callId, &item->data.func->argv[j].callId)) {
                    return SEMANTIC_CALL_RET_ERROR;
                }

                datatype_t provided = item->data.func->calls[i].params[j].attr.type;
                datatype_t required = item->data.func->argv[j].attr.type;

                if(!compare_datatypes(required, provided)) {
                    return SEMANTIC_CALL_RET_ERROR;
                }
            }
        } else {
            return SEMANTIC_CALL_RET_ERROR;
        }
    }

    return NO_ERRORS;
}

int statement_list() {
    SKIP_EOL();

    if (token.type == TYPE_EOF) {
        if (scope != 0 || inIf || inWhile) {
            // if function body, if or while body never reached } token
            return SYNTAX_ERROR;
        }

        // here we also have to check if all the functions that were called were also defined

        if (keysCnt == 0) return NO_ERRORS;


        for (int i = 0; i < keysCnt; i++) {
            item = symt_search(&gTable, &funcKeys[i]);
            if (!item->data.func->isDefined) return SEMANTIC_DEF_ERROR;
            EXEC(execute_calls());
        }
        keysCnt = 0;
        return NO_ERRORS;
    }

    if (token.type == TYPE_RBRACKET) {
        if (scope != 0) {
            if (inFunc) {
                if (currFunc->data.func->ret.type == NONE_DT || seenReturn) {
                    seenReturn = false;
                    inFunc = false;
                    symbstack_pop(&localTables);
                    scope--;

                    GET_TOKEN_SKIP_EOL();

                    return statement_list();
                } else {
                    return SEMANTIC_EXPR_ERROR;
                }
            } else if (inElse) {
                symbstack_pop(&localTables);
                scope--;

                inElse = false;

                GET_TOKEN_SKIP_EOL();

                return NO_ERRORS;
            } else if (inWhile) {
                inWhile--;
                symbstack_pop(&localTables);
                scope--;

                GET_TOKEN_SKIP_EOL();

                return NO_ERRORS;
            } else if (inIf) {
                inIf--;
                symbstack_pop(&localTables);
                scope--;

                NEXT_NON_EOL(token.type, TYPE_KW);
                RULE(expect_else());

                return statement_list();
            }
        } else return SYNTAX_ERROR;
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

    return statement_list();
}

int while_statement() {
    // token while here
    // TODO: connect expressions, for now just skip until { is found

    datatype_t resultDT;
    EXEC(parse_expression(1, &resultDT));

    inWhile++;
    scope++;
    EXEC(create_local_table());

    GET_TOKEN_SKIP_EOL();
    RULE(statement_list());

    return NO_ERRORS;
}

int func_def() {
    // check <func_header> first
    // token here is keyword with value K_FUNC
    NEXT_NON_EOL(token.type, TYPE_ID);

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

    currFunc = symt_search(&gTable, &token.attribute.id);

    string_t tmpFuncName;
    EXEC(str_create(&tmpFuncName, STR_SIZE));

    tmpFuncName = item->key;

    item->data.func->isDefined = true;
    // after this fork we will have function struct in item
    NEXT_NON_EOL(token.type, TYPE_LPAR);

    scope++;
    EXEC(create_local_table());
    item->scope = scope;

    RULE(parameters_list());

    if (token.type == TYPE_ARROW) {
        NEXT_NON_EOL(token.type, TYPE_KW);

        EXEC(kw_to_type(token.attribute.keyword, &item->data.func->ret));

        NEXT_NON_EOL(token.type, TYPE_LBRACKET);
    } else if (token.type == TYPE_LBRACKET) {
        item->data.func->ret.type = NONE_DT;
    } else return SYNTAX_ERROR;

    inFunc = true;

    RULE(func_body());
    inFunc = false;


    return NO_ERRORS;
}

// <parameters_list> ::= <parameter> <parameters_list_more> | ε
int parameters_list() {
    GET_TOKEN_SKIP_EOL();

    if (token.type == TYPE_RPAR) {
        GET_TOKEN_SKIP_EOL();
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
    GET_TOKEN_SKIP_EOL();
    if (token.type == TYPE_UNDERSCORE) { // second is underscore
        // Při použití _ jako identifikátor parametru se tento parametr v těle funkce nepoužívá.
        // not sure how to understand this, leaving this like that rn

        str_clear(&tmpTokenId);
        EXEC(str_append(&tmpTokenId, '_'));

    } else if (token.type == TYPE_ID) { // second is ID to use inside a function
        EXEC(str_copy(&token.attribute.id, &tmpTokenId));
        if (!str_cmp(&tmpTokenId, &toCall)) {
            return SEMANTIC_OTHER_ERROR;
        }
    } else {
        return SYNTAX_ERROR;
    }

    NEXT_NON_EOL(token.type, TYPE_COLON);
    NEXT_NON_EOL(token.type, TYPE_KW);

    symt_var_t tmp;
    tmp.mutable = false;

    EXEC(kw_to_type(token.attribute.keyword, &tmp.type));

    if (!str_cmp(&toCall, &tmpTokenId)) return SEMANTIC_OTHER_ERROR;

    EXEC(symt_add_func_param(item, &toCall, &tmpTokenId, tmp));

    htable *workingTable = localTables.head->table;

    // Add a variable to local function symtable
    ht_item_t *sVar = symt_search(workingTable, &tmpTokenId);

    if (sVar && sVar->type == var) {
        return SEMANTIC_DEF_ERROR;
    }

    EXEC(symt_add_var(workingTable, &tmpTokenId, tmp.type));

    sVar = symt_search(workingTable, &tmpTokenId);
    sVar->data.var->mutable = false;

    str_free(&toCall);

    GET_TOKEN_SKIP_EOL();
    RULE(parameters_list_more());

    return NO_ERRORS;
}

int parameters_list_more() {

    if (token.type == TYPE_COMMA) { // <parameters_list_more> ::= , <parameter> <parameters_list_more>
        GET_TOKEN_SKIP_EOL();
        RULE(parameter());
    } else if (token.type == TYPE_RPAR) {
        GET_TOKEN_SKIP_EOL();
    }

    return NO_ERRORS;
}

int func_body() {
    GET_TOKEN_SKIP_EOL();
    RULE(statement_list());
    return NO_ERRORS;
}

int return_statement() {
    // token here is keyword return

    GET_TOKEN_SKIP_EOL();

    if (token.type == TYPE_RBRACKET) {
        if (item->data.func->ret.type == NONE_DT) {
            return NO_ERRORS;
        }
    } else {
        // call expression analysis
        // EXEC(parse_expression(2));
        // get next token after }
        // GET_TOKEN();
        while (token.type != TYPE_RBRACKET) GET_TOKEN();
    }

    return NO_ERRORS;
}

int var_def() {
    // token here is either kw let or var
    symt_var_t var;


    var.type.type = NONE_DT;
    var.defined = false;


    if (token.attribute.keyword == K_LET) {
        var.mutable = false;
    } else if (token.attribute.keyword == K_VAR) {
        var.mutable = true;
    }

    NEXT_NON_EOL(token.type, TYPE_ID);

    ht_item_t *varItem;
    htable *workingTable;


    if (scope != 0) {
        workingTable = localTables.head->table;

        // check if we're not trying to redefine a variable in the same scope
        varItem = symt_search(workingTable, &token.attribute.id);
        if (varItem != NULL) return SEMANTIC_DEF_ERROR;

        varItem = symbstack_search(&localTables, &token.attribute.id);
    } else {
        varItem = symt_search(&gTable, &token.attribute.id);
        if (varItem != NULL) return SEMANTIC_DEF_ERROR;
        workingTable = &gTable;
    }

    EXEC(str_copy(&token.attribute.id, &tmpTokenId));

    GET_TOKEN_SKIP_EOL();

    if (token.type == TYPE_COLON) {
        NEXT_NON_EOL(token.type, TYPE_KW);
        EXEC(kw_to_type(token.attribute.keyword, &var.type));
        GET_TOKEN_SKIP_EOL();
    }

    if (token.type == TYPE_ASSIGN) {
        var.defined = true;
        datatype_t expressionResult;
        EXEC(parse_expression(0, &expressionResult));

        if (var.type.type != NONE_DT) {
            // if it's not none then we met declaration of datatype with `... : DATATYPE = ...`
            if (!compare_datatypes(expressionResult, var.type)) {
                return SEMANTIC_TYPE_ERROR;
            }
        } else {
            var.type = expressionResult;
        }
    }

    if (var.type.type == NONE_DT) {
        return SYNTAX_ERROR;
    } else if (var.type.nullable) {
        debug("Init var %s with nil", tmpTokenId.s);
    }

    EXEC(symt_add_var(workingTable, &tmpTokenId, var.type));

    varItem = symt_search(workingTable, &tmpTokenId);

    varItem->data.var->mutable = var.mutable;
    varItem->data.var->defined = var.defined;

    return NO_ERRORS;
}

int save_func_call_more() {

    if (token.type == TYPE_COMMA) {
        GET_TOKEN_SKIP_EOL();
        RULE(save_func_call_param());

        return NO_ERRORS;
    } else if (token.type == TYPE_RPAR) {
//        GET_TOKEN_SKIP_EOL();
//        GET_TOKEN();
        return NO_ERRORS;
    } else return SYNTAX_ERROR;

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
        EXEC(symt_add_func_call_param(item, NULL, &undersc, tmp));

        GET_TOKEN_SKIP_EOL();

        EXEC(save_func_call_more());

    } else if (token.type == TYPE_ID) {

        EXEC(str_copy(&token.attribute.id, &tmpTokenId));

        GET_TOKEN_SKIP_EOL();

        if (token.type == TYPE_COLON) {
            // if we found colon then next token is param and tmpTokenId is callId
//            NEXT_NON_EOL(token.type, TYPE_ID);

            // next token after colon is either ID or const which we are passing in function call
            GET_TOKEN_SKIP_EOL();

            if (token.type == TYPE_ID) {
                ht_item_t *var = find_var_in_symtables(&token.attribute.id);
                if (var == NULL) return SEMANTIC_UNDEF_VAR_ERROR;

                tmp.type = var->data.var->type;
                tmp.mutable = var->data.var->mutable;

                EXEC(symt_add_func_call_param(item, &tmpTokenId, &token.attribute.id, tmp));

            } else if (is_token_const(token.type)) {
                // token const here
                EXEC(token_type_to_datatype(token.type, &tmp.type));
                EXEC(symt_add_func_call_param(item, &tmpTokenId, NULL, tmp));

            } else return SYNTAX_ERROR;

            GET_TOKEN_SKIP_EOL();
            EXEC(save_func_call_more());

        } else if (token.type == TYPE_COMMA || token.type == TYPE_RPAR) {
            // tmpTokenId has parameter name
            ht_item_t *var = find_var_in_symtables(&tmpTokenId);
            if (var == NULL) return SEMANTIC_UNDEF_VAR_ERROR;

            tmp.type = var->data.var->type;
            tmp.mutable = var->data.var->mutable;

            EXEC(symt_add_func_call_param(item, NULL, &tmpTokenId, tmp));
            EXEC(save_func_call_more());
        } else return SYNTAX_ERROR;
    } else return SYNTAX_ERROR;

//    str_free(&undersc);

    return NO_ERRORS;
}

int save_func_call() {
    GET_TOKEN_SKIP_EOL();
    EXEC(symt_add_func_call(item));

    if (token.type == TYPE_RPAR) {
        EXEC(symt_zero_parameters_call(item));
    } else if (token.type == TYPE_ID || is_token_const(token.type)) {
        EXEC(save_func_call_param());
    } else {
        symt_remove_func_call(item);
        return SYNTAX_ERROR;
    }

    EXEC(append_func_keys(item->key));

    GET_TOKEN_SKIP_EOL();
    return NO_ERRORS;
}

int expression(){
    // token id here
    // save first token id
    EXEC(str_copy(&token.attribute.id, &tmpTokenId));

    // if the very next token is not TYPE_LPAR then it can't be function call
    // as it's forbidden to break func call with newlines, only `f(...` is allowed
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
    } else if (token.type == TYPE_ASSIGN) {
        // first check if we're not trying to access an undefined variable
        ht_item_t *var = find_var_in_symtables(&tmpTokenId);
        if (var == NULL) return SEMANTIC_UNDEF_VAR_ERROR;

        // TODO: in parse expression work with EOL skips
        datatype_t resultDT;
        EXEC(parse_expression(0, &resultDT));

        // TODO: check if we can use statement 5 + 5 as expression
    } else return SYNTAX_ERROR;

    return NO_ERRORS;
}

int call_parameters_list() {
    //<call_parameters_list> ::= <call_parameter> <call_parameters_list_more> | ε
    GET_TOKEN_SKIP_EOL();

    if (token.type == TYPE_RPAR) {
        // TODO: EXPAND FUNC CALL ! ! !
        if (item->data.func->argc == 0) {
            GET_TOKEN_SKIP_EOL();
            return NO_ERRORS;
        } else {
            return SEMANTIC_CALL_RET_ERROR;
        }
    } else if (token.type == TYPE_ID || is_token_const(token.type)) {
        RULE(call_parameter());
        return NO_ERRORS;
    } else {
        return SYNTAX_ERROR;
    }

}

int call_parameter() {
    // token here is either TYPE_ID or const
    int currArg = item->data.func->argPos;

    if (currArg >= item->data.func->argc) {
        if (!str_cmp_const(&item->key, "write")) {
            RULE(call_parameters_list_more());
            return NO_ERRORS;
        }
        return SEMANTIC_CALL_RET_ERROR;
    }

    if (is_token_const(token.type)) {
        // example: write("hello")
        datatype_t tmp;
        EXEC(token_type_to_datatype(token.type, &tmp));

        if (str_cmp_const(&item->data.func->argv[currArg].callId, "_")
            || !compare_datatypes(item->data.func->argv[currArg].attr.type, tmp)) {

            if (!str_cmp_const(&item->key, "write")) {
                RULE(call_parameters_list_more());
                return NO_ERRORS;
            }

            return SEMANTIC_CALL_RET_ERROR;
        }

    } else if (token.type == TYPE_ID) {

        if (!str_cmp_const(&item->data.func->argv[currArg].callId, "_")) {

            // if current argument was defined with _ as parameter name
            // then we can proceed with semantics control
            EXEC(check_call_param());

        } else {

            // if callId was defined then our parameter has to match this definition
            // save current ID, so we can check the next one
            EXEC(str_copy(&token.attribute.id, &tmpTokenId));
            if (str_cmp(&tmpTokenId, &item->data.func->argv[currArg].callId))
                return SEMANTIC_CALL_RET_ERROR;

            // return syntax error if next non EOL token is not comma
            // when it should be according to grammar rules
            NEXT_NON_EOL(token.type, TYPE_COLON);

            GET_TOKEN_SKIP_EOL();
            // here we can have two cases, for example: parameter "with: ID" or parameter "with: 5"
            if (is_token_const(token.type)) {
                // if it's const check datatype
                datatype_t tmp;
                EXEC(token_type_to_datatype(token.type, &tmp));
                if (!compare_datatypes(item->data.func->argv[currArg].attr.type, tmp)){
                    if (!str_cmp_const(&item->key, "write")) {
                        RULE(call_parameters_list_more());
                        return NO_ERRORS;
                    }
                    return SEMANTIC_CALL_RET_ERROR;
                }

            } else if (token.type == TYPE_ID) {
                // semantics control of a variable
                EXEC(check_call_param());

            } else return SYNTAX_ERROR;
        }
    }

    item->data.func->argPos++;

    // get next token (if no syntax error it should be , or ) token)
    RULE(call_parameters_list_more());
    return NO_ERRORS;
}

int call_parameters_list_more() {

    GET_TOKEN_SKIP_EOL();
    if (token.type == TYPE_COMMA) {
        GET_TOKEN_SKIP_EOL();
        RULE(call_parameter());
    } else if (token.type == TYPE_RPAR) {
        GET_TOKEN_SKIP_EOL();
        item->data.func->argPos = 0;
    } else {
        return SYNTAX_ERROR;
    }
    return NO_ERRORS;
}

int check_call_param() {
    ht_item_t *param = find_var_in_symtables(&token.attribute.id);
    if (param == NULL) return SEMANTIC_UNDEF_VAR_ERROR;

    // now we should check datatype of a variable we're passing into function
    if (!compare_datatypes(item->data.func->argv[item->data.func->argPos].attr.type, param->data.var->type)) {
        if (!str_cmp_const(&item->key, "write")) {
            return NO_ERRORS;
        }
        return SEMANTIC_CALL_RET_ERROR;
    }

    return NO_ERRORS;
}

int if_statement() {
    // kw if here
    GET_TOKEN_SKIP_EOL();

    // push new local symbtable to stack before we proceed with prec analysis

    inIf++;
    scope++;
    EXEC(create_local_table());


    if (token.type == TYPE_KW) {
        if (token.attribute.keyword != K_LET) return SYNTAX_ERROR;
        else {
            // get next non EOL token which has to be `id` for `let` statement
            NEXT_NON_EOL(token.type, TYPE_ID);
            // TODO: i guess save it to local symbtable, not sure
            htable *workingTable = localTables.head->table;
            datatype_t tmp;
            tmp.type = NONE_DT;
            tmp.nullable = false;
            EXEC(symt_add_var(workingTable, &token.attribute.id, tmp));

            ht_item_t *var = symt_search(workingTable, &token.attribute.id);
            var->data.var->mutable = false;

            // next non-whitespace has to be {, otherwise syntax error
            NEXT_NON_EOL(token.type, TYPE_LBRACKET);
        }
    } else {
        // check for empty expression
        datatype_t resultDT;
        EXEC(parse_expression(2, &resultDT));
    }

    GET_TOKEN_SKIP_EOL();
    RULE(statement_list());
    return NO_ERRORS;
}

int expect_else() {
    if (token.attribute.keyword != K_ELSE) return SYNTAX_ERROR;

    NEXT_NON_EOL(token.type, TYPE_LBRACKET);

    inElse = true;

    scope++;
    EXEC(create_local_table());

    GET_TOKEN_SKIP_EOL();
    RULE(statement_list());
    return NO_ERRORS;
}

int parse() {
    symt_init(&gTable);
    EXEC(str_create(&token.attribute.id, STR_SIZE));
    EXEC(str_create(&tmpTokenId, STR_SIZE));

    EXEC(init_builtins());

    EXEC(init_func_keys());
    // Get first token
    GET_TOKEN_SKIP_EOL();

    // We are in the main body of a program
    code = statement_list();

    // Memory cleaning

    symt_free(&gTable);
    str_clear(&token.attribute.id);
    str_clear(&tmpTokenId);
    symbstack_free(&localTables);
    debug("after cleaning");

    return code;
}