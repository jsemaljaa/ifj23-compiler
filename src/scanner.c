/*
 * compiler.c
 *
 * @brief Implementation of lexical analysis
 * @author Alina Vinogradova <xvinog00@vutbr.cz>
 */

#include "scanner.h"

// Counter for hex in \u escape sequence
int hexCnt = 0;
bool comment = false;

bool match_keyword(token_t *token) {
    static const char *keywords[] = {
            "Double",
            "else",
            "func",
            "if",
            "Int",
            "let",
            "nil",
            "return",
            "String",
            "var",
            "while"
    };
    for (keyword_t i = K_DOUBLE; i < K_WHILE; i++) {
        if (str_cmp_const(&token->attribute.id, keywords[i])) {
            token->attribute.keyword = i;
            token->type = TYPE_KW;
            str_clear(&token->attribute.id);
            return true;
        }
    }

    return false;
}

states_t process_char(token_t *token, char c) {
    states_t state = STATE_START;
    if (c == EOF) {
        token->type = TYPE_EOF;
    } else if (c == '\n') {
        token->type = TYPE_EOL;
    } else if (c == ':') {
        token->type = TYPE_COLON;
    } else if (c == '.') {
        token->type = TYPE_DOT;
    } else if (c == ',') {
        token->type = TYPE_COMMA;
    } else if (c == '_') {
        state = STATE_UNDERSCORE;
    } else if (c == '{') {
        token->type = TYPE_LBRACKET;
    } else if (c == '}') {
        token->type = TYPE_RBRACKET;
    } else if (c == '(') {
        token->type = TYPE_LPAR;
    } else if (c == ')') {
        token->type = TYPE_RPAR;
    } else if (c == '*') {
        token->type = TYPE_MUL;
    } else if (c == '/') {
        state = STATE_DIV;
    } else if (c == '-') {
        state = STATE_MINUS;
    } else if (c == '+') {
        token->type = TYPE_PLUS;
    } else if (c == '>') {
        state = STATE_GREATER;
    } else if (c == '<') {
        state = STATE_LESS;
    } else if (c == '!') {
        state = STATE_EXCL;
    } else if (c == '?') {
        state = STATE_QUES;
    } else if (isupper(c) || islower(c)) {
        // check for other error 99
        str_append(&token->attribute.id, c);
        state = STATE_ID_KW;
    } else if (c == '"') {
        state = STATE_STRING_START;
    } else if (isspace(c)){
        state = STATE_START;
    } else if (isdigit(c)) {
        // check for other error 99
        str_append(&token->attribute.id, c);
        state = STATE_NUMBER_INTEGER;
    }
    return state;
}

int get_token(token_t *token){
    str_clear(&token->attribute.id);

    states_t state = STATE_START;
    char c;

    while (1) {
        c = getc(stdin);

        switch (state) {

            case STATE_START:
                state = process_char(token, c);
                break;

            case STATE_UNDERSCORE:
                str_append(&token->attribute.id, c);
                c = getc(stdin);
                if (isupper(c) || islower(c) || isdigit(c)) {
                    str_append(&token->attribute.id, c);
                    state = STATE_ID_KW;
                    break;
                } else {
                    // should probably add here
                    str_clear(&token->attribute.id);
                    ungetc(c, stdin);
                    token->type = TYPE_UNDERSCORE;
                    return NO_ERRORS;
                }

            case STATE_MINUS:
                c = getc(stdin);
                if (c == '>') {
                    token->type = TYPE_ARROW;
                    return NO_ERRORS;
                } else {
                    ungetc(c, stdin);
                    token->type = TYPE_MINUS;
                    return NO_ERRORS;
                }

            case STATE_DIV:
                c = getc(stdin);
                if (c == '*') {
                    state = STATE_COMM_BLOCK_START;
                    break;
                } else if (c == '/') {
                    state = STATE_COMM_LINE;
                    break;
                } else if (c == EOF || c == '\n') {
                    return LEXICAL_ERROR;
                } else {
                    ungetc(c, stdin);
                    token->type = TYPE_DIV;
                    return NO_ERRORS;
                }

            case STATE_GREATER:
                c = getc(stdin);
                if (c == '=') {
                    token->type = TYPE_GE;
                    return NO_ERRORS;
                } else {
                    ungetc(c, stdin);
                    token->type = TYPE_GT;
                    return NO_ERRORS;
                }

            case STATE_LESS:
                c = getc(stdin);
                if (c == '=') {
                    token->type = TYPE_LE;
                    return NO_ERRORS;
                } else {
                    ungetc(c, stdin);
                    token->type = TYPE_LT;
                    return NO_ERRORS;
                }

            case STATE_EXCL:
                c = getc(stdin);
                if (c == '=') {
                    token->type = TYPE_NEQ;
                    return NO_ERRORS;
                } else {
                    ungetc(c, stdin);
                    token->type = TYPE_EXCL;
                    return NO_ERRORS;
                }

            case STATE_QUES:
                c = getc(stdin);
                if (c == '?') {
                    token->type = TYPE_NILCOAL;
                    return NO_ERRORS;
                } else {
                    ungetc(c, stdin);
                    token->type = TYPE_QUES;
                    return NO_ERRORS;
                }

            case STATE_ID_KW:
                if (c == '_' || isupper(c) || islower(c) || isdigit(c)) {
                    if (!str_append(&token->attribute.id, c)) {
                        return OTHER_ERROR;
                    }
                } else {
                    if (!match_keyword(token)) {
                        token->type = TYPE_ID;
                    }
                    return NO_ERRORS;
                }
                break;

            case STATE_STRING_START:
                if (c == '"') {
                    state = STATE_STRING_END;
                    break;
                } else if (c == '\\') {
                    if (!str_append(&token->attribute.string, c)) {
                        return OTHER_ERROR;
                    }
                    state = STATE_STRING_ESCAPE;
                    break;
                } else if (c == EOF || c < 32) {
                    return LEXICAL_ERROR;
                } else {
                    if (!str_append(&token->attribute.string, c)) {
                       return OTHER_ERROR;
                    }
                }

            case STATE_STRING_END:
                if (c == '"') {
                    state = STATE_STRING_MULTILINE;
                    break;
                } else {
                    token->type = TYPE_STRING;
                    return NO_ERRORS;
                }

            case STATE_STRING_ESCAPE:
                if (c == '"' || c == 'n' || c == 'r' || c == 't' || c == '\\') {
                    if (!str_append(&token->attribute.string, c)) {
                        return OTHER_ERROR;
                    }
                    state = STATE_STRING_START;
                    break;
                } else if (c == 'u') {
                    if (!str_append(&token->attribute.string, c)) {
                        return OTHER_ERROR;
                    }
                    state = STATE_STRING_SEQ;
                    break;
                }

            case STATE_STRING_SEQ:
                if (c == '{') {
                    if (!str_append(&token->attribute.string, c)) {
                        return OTHER_ERROR;
                    }
                    state = STATE_STRING_SEQ_HEX;
                }
                break;
            case STATE_STRING_SEQ_HEX:
                if (c == '}') {
                    if (hexCnt == 0) {
                        return LEXICAL_ERROR;
                    }
                    if (!str_append(&token->attribute.string, c)) {
                        return OTHER_ERROR;
                    }
                    hexCnt = 0;
                    state = STATE_STRING_START;
                } else if ((c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f') || (c >= '0' && c <= '9')) {
                    if (hexCnt > 8) {
                        return LEXICAL_ERROR;
                    }

                    if (!str_append(&token->attribute.string, c)) {
                        return OTHER_ERROR;
                    }
                    hexCnt++;
                    // counter so max 8 symbols are appended to the string
                }
                break;

            case STATE_COMM_BLOCK_END:
                break;

            case STATE_STRING_MULTILINE:
                break;


            case STATE_NUMBER_INTEGER:
                if (c == 'e' || c == 'E') {
                    state = STATE_NUMBER_EXP_START;
                } else if (c == '.') {
                    state = STATE_NUMBER_DOUBLE_START;
                } else if (!isdigit(c)) {
                    // 
                }
                break;
            case STATE_NUMBER_DOUBLE_START:
                break;
            case STATE_NUMBER_DOUBLE_END:
                break;
            case STATE_NUMBER_EXP_START:
                break;
            case STATE_NUMBER_EXP_SIGN:
                break;
            case STATE_NUMBER_EXP_END:
                break;
            case STATE_COMM_BLOCK_START:
                break;
            case STATE_COMM_LINE:
                break;
        }

    }

    return NO_ERRORS;
}
