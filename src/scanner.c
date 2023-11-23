/*
 * Project: IFJ23 compiler
 *
 * scanner.c
 *
 * @brief Implementation of lexical analysis
 *
 * @author Alina Vinogradova <xvinog00@vutbr.cz>
 */

#include "scanner.h"

// Counter for hex in \u escape sequence
int hexCnt = 0;
int commCnt = 0;

int multiline = 0;
bool endOfLine = false;

bool metExcl;

states_t state;

bool peek_next_two() {
    int c = getc(stdin);
    int cc = getc(stdin);

    if (c == '"' && cc == '"') {
        ungetc(cc, stdin);
        ungetc(c, stdin);
        return true;
    }
    return false;
}

keyword_t match_keyword(token_t *token) {
    static const char *keywords[] = {
            "Int",
            "Double",
            "String",
            "Int?",
            "Double?",
            "String?",
            "nil",
            "else",
            "func",
            "if",
            "let",
            "return",
            "var",
            "while"
    };

    for (keyword_t i = K_INT; i < K_NONE; i++) {
        if (!str_cmp_const(&token->attribute.id, keywords[i])) {
            return i;
        }
    }

    return K_NONE;
}

int get_token(token_t *token){
    str_clear(&token->attribute.id);

    state = STATE_START;
    char c;

    metExcl = false;

    while (1) {
        c = getc(stdin);

        switch (state) {

            case STATE_START:
                if (c == EOL) {
                    token->type = TYPE_EOL;
                    return NO_ERRORS;
                } else if (c == EOF) {
                    token->type = TYPE_EOF;
                    return NO_ERRORS;
                } else if (c == ':') {
                    token->type = TYPE_COLON;
                    return NO_ERRORS;
                } else if (c == '.') {
                    token->type = TYPE_DOT;
                    return NO_ERRORS;
                } else if (c == ',') {
                    token->type = TYPE_COMMA;
                    return NO_ERRORS;
                } else if (c == '{') {
                    token->type = TYPE_LBRACKET;
                    return NO_ERRORS;
                } else if (c == '}') {
                    token->type = TYPE_RBRACKET;
                    return NO_ERRORS;
                } else if (c == '(') {
                    token->type = TYPE_LPAR;
                    return NO_ERRORS;
                } else if (c == ')') {
                    token->type = TYPE_RPAR;
                    return NO_ERRORS;
                } else if (c == '*') {
                    token->type = TYPE_MUL;
                    return NO_ERRORS;
                } else if (c == '+') {
                    token->type = TYPE_PLUS;
                    return NO_ERRORS;
                } else if (c == '=') {
                    state = STATE_EQUALS;
                } else if (c == '_') {
                    EXEC_STR(str_append(&token->attribute.id, c));
                    state = STATE_UNDERSCORE;
                } else if (c == '-') {
                    state = STATE_MINUS;
                } else if (c == '/') {
                    state = STATE_DIV;
                }  else if (c == '>') {
                    state = STATE_GREATER;
                } else if (c == '<') {
                    state = STATE_LESS;
                } else if (c == '!') {
                    state = STATE_EXCL;
                } else if (c == '?') {
                    state = STATE_QUES;
                } else if (isupper(c) || islower(c)) {
                    EXEC_STR(str_append(&token->attribute.id, c));
                    state = STATE_ID_KW;
                } else if (c == '"') {
                    state = STATE_STRING_START;
                } else if (isspace(c)){
                    state = STATE_START;
                } else if (isdigit(c)) {
                    EXEC_STR(str_append(&token->attribute.id, c));
                    state = STATE_NUMBER_INTEGER;
                } else return LEXICAL_ERROR;
                break;

            case STATE_EQUALS:
                if (c == '=') {
                    token->type = TYPE_EQ;
                } else {
                    ungetc(c, stdin);
                    token->type = TYPE_ASSIGN;
                }
                return NO_ERRORS;

            case STATE_UNDERSCORE:
                if (isupper(c) || islower(c) || isdigit(c)) {
                    EXEC_STR(str_append(&token->attribute.id, c));
                    state = STATE_ID_KW;
                } else {
                    // should probably add here
                    str_clear(&token->attribute.id);
                    ungetc(c, stdin);
                    token->type = TYPE_UNDERSCORE;
                    return NO_ERRORS;
                }
                break;

            case STATE_MINUS:
                if (c == '>') {
                    token->type = TYPE_ARROW;
                } else {
                    ungetc(c, stdin);
                    token->type = TYPE_MINUS;
                }
                return NO_ERRORS;

            case STATE_DIV:
                if (c == '*') {
                    commCnt++;
                    state = STATE_COMM_BLOCK_START;
                } else if (c == '/') {
                    state = STATE_COMM_LINE;
                } else if (c == EOF || c == EOL) {
                    return LEXICAL_ERROR;
                } else {
                    ungetc(c, stdin);
                    token->type = TYPE_DIV;
                    return NO_ERRORS;
                }
                break;

            case STATE_GREATER:
                if (c == '=') {
                    token->type = TYPE_GE;
                } else {
                    ungetc(c, stdin);
                    token->type = TYPE_GT;
                }
                return NO_ERRORS;

            case STATE_LESS:
                if (c == '=') {
                    token->type = TYPE_LE;
                } else {
                    ungetc(c, stdin);
                    token->type = TYPE_LT;
                }
                return NO_ERRORS;

            case STATE_EXCL:
                if (c == '=') {
                    token->type = TYPE_NEQ;
                } else {
                    ungetc(c, stdin);
                    token->type = TYPE_EXCL;
                }
                return NO_ERRORS;

            case STATE_QUES:
                if (c == '?') {
                    token->type = TYPE_NILCOAL;
                    return NO_ERRORS;
                } else {
                    return LEXICAL_ERROR;
                }

            case STATE_ID_KW:
                if (metExcl) {
                    // exclamation mark should be the end of keyword, can't be seen in an id
                    keyword_t kw = match_keyword(token);
                    ungetc(c, stdin);
                    if (kw == K_NONE) return LEXICAL_ERROR;
                }

                if (c == '_' || isupper(c) || islower(c) || isdigit(c) || c == '?') {
                    if (c == '?') metExcl = true;
                    EXEC_STR(str_append(&token->attribute.id, c));
                } else {
                    keyword_t kw = match_keyword(token);
                    ungetc(c, stdin);
                    if (kw == K_NONE) {
                        token->type = TYPE_ID;
                        return NO_ERRORS;
                    } else {
                        str_clear(&token->attribute.id);
                        token->attribute.keyword = kw;
                        token->type = TYPE_KW;
                    }

                    return NO_ERRORS;
                }
                break;

            case STATE_STRING_START:
                if (c == '"') {
                    state = STATE_STRING_END;
                } else if (c == '\\') {
                    EXEC_STR(str_append(&token->attribute.string, c));
                    state = STATE_STRING_ESCAPE;
                } else if (c == EOF || c < 32) {
                    return LEXICAL_ERROR;
                } else {
                    EXEC_STR(str_append(&token->attribute.string, c));
                }
                break;

            case STATE_STRING_END:
                if (c == '"') {
                    state = STATE_STRING_MULTILINE_START;
                    break;
                } else {
                    ungetc(c, stdin);
                    token->type = TYPE_STRING;
                    return NO_ERRORS;
                }

            case STATE_STRING_MULTILINE_START:
                // here we got first symbol of multiline string from input, then we can start reading string
                if (c != EOL) return LEXICAL_ERROR;
                state = STATE_STRING_MULTILINE_READ;
                break;

            case STATE_STRING_MULTILINE_READ:
                // here we are already inside of a multiline string """
                // in multiline read we're always only at the beginning of the multiline string
//                if (c == '"') {
//                    multiline++; // first "
//                    EXEC_STR(str_append(&token->attribute.string, c));
//                    state = STATE_STRING_MULTILINE_READ_MORE;
//                } else {
//                    EXEC_STR(str_append(&token->attribute.string, c));
//                }
                // multiline
                break;

//            case STATE_STRING_MULTILINE_READ_MORE:
//                if (c == '"') {
//                    multiline++;
//                    EXEC_STR(str_append(&token->attribute.string, c));
//                } else {
//                    multiline = 0;
//                    EXEC_STR(str_append(&token->attribute.string, c));
//                }

            case STATE_STRING_MULTILINE_END:
                break;

            case STATE_STRING_ESCAPE:
                if (c == '"' || c == 'n' || c == 'r' || c == 't' || c == '\\') {
                    EXEC_STR(str_append(&token->attribute.string, c));
                    state = STATE_STRING_START;
                } else if (c == 'u') {
                    EXEC_STR(str_append(&token->attribute.string, c));
                    state = STATE_STRING_SEQ;
                } else return LEXICAL_ERROR;
                break;

            case STATE_STRING_SEQ:
                if (c == '{') {
                    EXEC_STR(str_append(&token->attribute.string, c));
                    state = STATE_STRING_SEQ_HEX;
                } else return LEXICAL_ERROR;
                break;

            case STATE_STRING_SEQ_HEX:
                if (c == '}') {
                    if (hexCnt == 0) {
                        return LEXICAL_ERROR;
                    }
                    EXEC_STR(str_append(&token->attribute.string, c));
                    hexCnt = 0;
                    state = STATE_STRING_START;
                } else if ((c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f') || (c >= '0' && c <= '9')) {
                    hexCnt++;
                    if (hexCnt > 8) {
                        return LEXICAL_ERROR;
                    }
                    EXEC_STR(str_append(&token->attribute.string, c));
                    // counter so max 8 symbols are appended to the string
                } else return LEXICAL_ERROR;
                break;

            case STATE_NUMBER_INTEGER:
                if (c == 'e' || c == 'E') {
                    EXEC_STR(str_append(&token->attribute.string, c));
                    state = STATE_NUMBER_EXP_START;
                } else if (c == '.') {
                    EXEC_STR(str_append(&token->attribute.string, c));
                    state = STATE_NUMBER_DOUBLE_START;
                } else if (!isdigit(c)) {
                    ungetc(c, stdin);
                    token->attribute.integerNumber = atoi(token->attribute.string.s);
                    token->type = TYPE_INT;
                    return NO_ERRORS;
                } else {
                    EXEC_STR(str_append(&token->attribute.string, c));
                }
                break;

            case STATE_NUMBER_DOUBLE_START:
                if (isdigit(c)) {
                    EXEC_STR(str_append(&token->attribute.string, c));
                    state = STATE_NUMBER_DOUBLE_END;
                } else {
                    return LEXICAL_ERROR;
                }
                break;

            case STATE_NUMBER_DOUBLE_END:
                if (isdigit(c)) {
                    EXEC_STR(str_append(&token->attribute.string, c));
                } else {
                    ungetc(c, stdin);
                    token->attribute.floatNumber = atof(token->attribute.string.s);
                    token->type = TYPE_DOUBLE;
                    return NO_ERRORS;
                }
                break;

            case STATE_NUMBER_EXP_START:
                if (c == '+' || c == '-') {
                    EXEC_STR(str_append(&token->attribute.string, c));
                    state = STATE_NUMBER_EXP_SIGN;
                } else if (isdigit(c)) {
                    EXEC_STR(str_append(&token->attribute.string, c));
                    state = STATE_NUMBER_EXP_END;
                } else {
                    return LEXICAL_ERROR;
                }

                break;

            case STATE_NUMBER_EXP_SIGN:
                if (isdigit(c)) {
                    EXEC_STR(str_append(&token->attribute.string, c));
                    state = STATE_NUMBER_EXP_END;
                } else {
                    return LEXICAL_ERROR;
                }
                break;

            case STATE_NUMBER_EXP_END:
                if (isdigit(c)) {
                    EXEC_STR(str_append(&token->attribute.string, c));
                } else {
                    ungetc(c, stdin);
                    token->attribute.integerNumber = atof(token->attribute.string.s);
                    token->type = TYPE_DOUBLE;
                    return NO_ERRORS;
                }
                break;

            case STATE_COMM_LINE:
                while (true) {
                    if (c == EOF) {
                        ungetc(c, stdin);
                        break;
                    } else if (c == EOL) {
                        break;
                    } else {
                        c = getc(stdin);
                    }
                }
                state = STATE_START;
                break;

            case STATE_COMM_BLOCK_START: // /*
                if (c == '*') {
                    state = STATE_COMM_BLOCK_END;
                } else if (c == EOF) {
                    return LEXICAL_ERROR;
                }
                break;

            case STATE_COMM_BLOCK_END:
                if (c == '/') {
                    commCnt--;
                    if (commCnt == 0) {
                        state = STATE_START;
                    } else {
                        state = STATE_COMM_BLOCK_START;
                    }
                } else if (c == EOF) {
                    return LEXICAL_ERROR;
                } else {
                    state = STATE_COMM_BLOCK_START;
                }
                break;
        }

    }
    return code;
}
