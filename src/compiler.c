/*
 * compiler.c
 *
 * @brief Main module
 * @author Alina Vinogradova <xvinog00@vutbr.cz>
 */

#include "scanner.h"

const char* token_type_to_string(token_type_t type) {
    switch (type) {
        case TYPE_EOF:
            return "End of file";
        case TYPE_EOL:
            return "End of line";
        case TYPE_COLON:
            return ":";
        case TYPE_DOT:
            return ".";
        case TYPE_COMMA:
            return ",";
        case TYPE_UNDERSCORE:
            return "_";
        case TYPE_LBRACKET:
            return "{";
        case TYPE_RBRACKET:
            return "}";
        case TYPE_LPAR:
            return "(";
        case TYPE_RPAR:
            return ")";
        case TYPE_ARROW:
            return "->";
        case TYPE_MUL:
            return "*";
        case TYPE_DIV:
            return "/";
        case TYPE_MINUS:
            return "-";
        case TYPE_PLUS:
            return "+";
        case TYPE_GT:
            return ">";
        case TYPE_GE:
            return ">=";
        case TYPE_LT:
            return "<";
        case TYPE_LE:
            return "<=";
        case TYPE_NEQ:
            return "!=";
        case TYPE_EQ:
            return "==";
        case TYPE_ASSIGN:
            return "=";
        case TYPE_EXCL:
            return "!";
        case TYPE_QUES:
            return "?";
        case TYPE_NILCOAL:
            return "??";
        case TYPE_INT:
            return "Int";
        case TYPE_DOUBLE:
            return "Double";
        case TYPE_STRING:
            return "String";
        case TYPE_ID:
            return "identifier";
        case TYPE_KW:
            return "keyword";
        case TYPE_DBG:
            return "debug";
        default:
            return "Unknown";
    }
}

const char* keyword_to_string(keyword_t keyword) {
    switch (keyword) {
        case K_DOUBLE:
            return "Double";
        case K_ELSE:
            return "else";
        case K_FUNC:
            return "func";
        case K_IF:
            return "if";
        case K_INT:
            return "Int";
        case K_LET:
            return "let";
        case K_NIL:
            return "nil";
        case K_RETURN:
            return "return";
        case K_STRING:
            return "String";
        case K_VAR:
            return "var";
        case K_WHILE:
            return "while";
        case K_INT_N:
            return "Int?";
        case K_STRING_N:
            return "String?";
        case K_DOUBLE_N:
            return "Double?";
        default:
            return "Unknown";
    }
}


int main(){
    // currently used for dynamic strings testing
    /*
    string_t str;
    str_create(&str, 20);
    printf("Allocated size: %d\n", str.allocated);

    str_append(&str, 'a');
    str_append(&str, 'c');
    printf("String: %s\n", str.s);

    string_t t;
    str_create(&t, 1);
    str_copy(&str, &t);
    printf("Test string: %s\n", t.s);

    str_append(&t, 'e');
    str_append(&t, 'f');
    str_concat(&str, &t);
    printf("Result string: %s\n", str.s);
    */

    token_t token;
    str_create(&token.attribute.id, 20);
    int tokenCnt = 0;
    int ret = NO_ERRORS;
    while (token.type != TYPE_EOF){
        ret = get_token(&token);
        tokenCnt++;
        if (ret != LEXICAL_ERROR) {
            printf("[%d] --- Token is %s\n", tokenCnt, token_type_to_string(token.type));
            if (token.type == TYPE_STRING) {
                printf("------------ String contains: '%s'\n", token.attribute.string.s);
            } else if (token.type == TYPE_ID) {
                printf("------------ ID contains: '%s'\n", token.attribute.id.s);
            } else if (token.type == TYPE_KW) {
                printf("------------ Keyword contains: '%s'\n", keyword_to_string(token.attribute.keyword));
            } else if (token.type == TYPE_INT) {
                printf("------------ Token value: '%d'\n", token.attribute.integerNumber);
            }

        } else {
            printf("LEXICAL ERROR\n");
            return ret;
        }
    }

    return ret;
}