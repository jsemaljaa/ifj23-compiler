#ifndef UTILS_H
#define UTILS_H

#include "scanner.h"
#include "log.h"

#define EXPECT_ERROR(code)      \
    if (code != NO_ERRORS) return code;  \

#define EXEC(func) \
    code = (func); \
    EXPECT_ERROR(code); \
    debug("Executing %s", #func); \

#define RULE(func)                      \
    code = (func);                      \
    EXPECT_ERROR(code);                 \
    debug("Applying rule %s", #func);   \

#define GET_TOKEN()                 \
    code = get_token(&token);          \
    debug("Token: %s", token_type_to_string(token.type)); \
    if (token.type == TYPE_KW) debug("Keyword value: %s", keyword_to_string(token.attribute.keyword));                                \
    EXPECT_ERROR(code);                                \


#define EXPECT(current, expected) \
    if(current != expected) { \
        error("Unexpected token %s", token_type_to_string(current)); \
        exit(2); \
    } \

__attribute__((unused))
static const char* token_type_to_string(token_type_t type) {
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

__attribute__((unused))
static const char* keyword_to_string(keyword_t keyword) {
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

#endif // UTILS_H