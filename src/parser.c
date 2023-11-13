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
    debug("<statement_list> ::= <statement> <statement_list>");
    if (token.type == TYPE_EOF) {
        return inFunc ? SYNTAX_ERROR : NO_ERRORS;
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
            inFunc = false;
            return NO_ERRORS;
        }
    } else if (token.type == TYPE_ID) {
        debug("<statement> ::= <expression>");
        RULE(expression()); // expression or func call
    } else if (token.type == TYPE_KW) {
        switch (token.attribute.keyword) {
            case K_FUNC:
            debug("<statement> ::= <func_def>");
                if (!inFunc) {
                    RULE(func_def());
                } else {
                    return SEMANTIC_DEF_ERROR;
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
        if (token.type == TYPE_ARROW) {
            GET_TOKEN();
            EXPECT(token.type, TYPE_KW);
            int type = kw_to_type(token.attribute.keyword);
            if (type == OTHER_ERROR) return SYNTAX_ERROR;
            item->data.func->ret = type;
            GET_TOKEN();
            EXPECT(token.type, TYPE_LBRACKET);
            RULE(func_body());
        } else if (token.type == TYPE_LBRACKET) {
            item->data.func->ret = NONE_DT;
            RULE(func_body());
        }
        return NO_ERRORS;
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

int func_body() {
    inFunc = true;
    GET_TOKEN();
    RULE(statement_list());
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
    inFunc = false;
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
