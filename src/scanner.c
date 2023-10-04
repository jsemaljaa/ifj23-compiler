/*
 * compiler.c
 *
 * @brief Implementation of lexical analysis
 * @author Alina Vinogradova <xvinog00@vutbr.cz>
 */

#include "scanner.h"

void match_keyword(token_t *token) {
    for (keyword_t i = K_DOUBLE; i < K_WHILE; i++) {
        if (str_cmp_const(&token->attribute.id, kws[i])) {
            token->attribute.keyword = i;
            token->type = TYPE_KW;
            str_clear(&token->attribute.id);
        }
    }
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
        str_append(&token->attribute.id, c);
        state = STATE_ID_KW;
    } else if (isspace(c)){
        state = STATE_START;
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
                } else {
                    // should probably add here
                    str_clear(&token->attribute.id);
                    ungetc(c, stdin);
                    token->type = TYPE_UNDERSCORE;
                }
                break;

            case STATE_MINUS:
                c = getc(stdin);
                if (c == '>') {
                    token->type = TYPE_ARROW;
                } else {
                    ungetc(c, stdin);
                    token->type = TYPE_UNDERSCORE;
                }
                break;

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
                }
                break;

            case STATE_GREATER:
                c = getc(stdin);
                if (c == '=') {
                    token->type = TYPE_GE;
                } else {
                    ungetc(c, stdin);
                    token->type = TYPE_GT;
                }
                break;

            case STATE_LESS:
                c = getc(stdin);
                if (c == '=') {
                    token->type = TYPE_LE;
                } else {
                    ungetc(c, stdin);
                    token->type = TYPE_LT;
                }
                break;

            case STATE_EXCL:
                c = getc(stdin);
                if (c == '=') {
                    token->type = TYPE_NEQ;
                } else {
                    ungetc(c, stdin);
                    token->type = TYPE_EXCL;
                }
                break;

            case STATE_QUES:
                c = getc(stdin);
                if (c == '?') {
                    token->type = TYPE_NILCOAL;
                } else {
                    ungetc(c, stdin);
                    token->type = TYPE_QUES;
                }
                break;

            case STATE_ID_KW:
                int scanid = 1;
                while (scanid) {
                    c = getc(stdin);
                    if (c == '_' || isupper(c) || islower(c) || isdigit(c)) {
                        str_append(&token->attribute.id, c);
                    } else {
                        scanid = 0;
                        ungetc(c, stdin);
                    }
                }
                match_keyword(token);
                break;

            case STATE_STRING_START:
                break;
            case STATE_COMM_BLOCK_END:
                break;
            case STATE_STRING_END:
                break;
            case STATE_STRING_MULTILINE:
                break;
            case STATE_STRING_ESCAPE:
                break;
            case STATE_STRING_SEQ:
                break;
            case STATE_STRING_SEQ_HEX:
                break;
            case STATE_NUMBER_INTEGER:
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
