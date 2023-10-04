/*
 * compiler.c
 *
 * @brief Implementation of lexical analysis
 * @author Alina Vinogradova <xvinog00@vutbr.cz>
 */

#include "scanner.h"

void match_keyword(token_t *token) {
    for (keyword_t i = K_DOUBLE; i < K_WHILE; i++) {
        if (str_cmp_const(&token->attribute.id, keywords[i])) {
            token->attribute.keyword = i;
            token->type = TYPE_KW;
            str_clear(&token->attribute.id);
        }
    }
}

int get_token(token_t *token){
    int ret = NO_ERRORS;
    str_clear(&token->attribute.id);

    states_t state = STATE_START;
    char c;

    while (true) {
        c = getc(stdin);

        switch (state) {
            case STATE_START:
                switch (c) {
                    case EOF: token->type = TYPE_EOF;
                    case '\n': token->type = TYPE_EOL;
                    case ':': token->type = TYPE_COLON;
                    case '.': token->type = TYPE_DOT;
                    case ',': token->type = TYPE_COMMA;
                    case '_': state = STATE_UNDERSCORE;
                    case '{': token->type = TYPE_LBRACKET;
                    case '}': token->type = TYPE_RBRACKET;
                    case '(': token->type = TYPE_LPAR;
                    case ')': token->type = TYPE_RPAR;
                    case '*': token->type = TYPE_MUL;
                    case '/': state = STATE_DIV;
                    case '-': state = STATE_MINUS;
                    case '+': token->type = TYPE_PLUS;
                    case '>': state = STATE_GREATER;
                    case '<': state = STATE_LESS;
                    case '!': state = STATE_EXCL;
                    case '?': state = STATE_QUES;


                    case isupper(c):
                    case islower(c):
                        str_append(&token->attribute.id, c);
                        state = STATE_ID_KW;
                    case isspace(c):
                        state = STATE_START;
                }

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
                    token->type = TOKEN_UNDERSCORE;
                }


            case STATE_MINUS:
                c = getc(stdin);
                if (c == '>') {
                    token->type = TYPE_ARROW;
                } else {
                    ungetc(c, stdin);
                    token->type = TYPE_UNDERSCORE;
                }

            case STATE_COMM_BLOCK_START:
            case STATE_COMM_LINE:
                token->type = TYPE_DBG;

            case STATE_DIV:
                c = getc(stdin);
                if (c == '*') {
                    state = STATE_COMM_BLOCK_START;
                    break;
                } else if (c == '/') {
                    state = STATE_COMM_LINE;
                    break;
                } else if (c == EOF || c == '\n') {
                    ret = LEXICAL_ERROR;
                    return ret;
                } else {
                    ungetc(c, stdin);
                    token->type = TYPE_DIV;
                }

            case STATE_GREATER:
                c = getc(stdin);
                if (c == '=') {
                    token->type = TYPE_GE;
                } else {
                    ungetc(c, stdin);
                    token->type = TYPE_GT;
                }

            case STATE_LESS:
                c = getc(stdin);
                if (c == '=') {
                    token->type = TYPE_LE;
                } else {
                    ungetc(c, stdin);
                    token->type = TYPE_LT;
                }

            case STATE_EXCL:
                c = getc(stdin);
                if (c == '=') {
                    token->type = TYPE_NEQ;
                } else {
                    ungetc(c, stdin);
                    token->type = TYPE_EXCL;
                }

            case STATE_QUES:
                c = getc(stdin);
                if (c == '?') {
                    token->type = TYPE_NILCOAL;
                } else {
                    ungetc(c, stdin);
                    token->type = TYPE_QUES;
                }
            return ret;
            case STATE_COMM_BLOCK_END:
                break;
            case STATE_ID_KW:
                bool scanid = true;
                while (scanid) {
                    c = getc(stdin);
                    if (c == '_' || isupper(c) || islower(c) || isdigit(c)) {
                        str_append(&token->attribute.id, c);
                    } else {
                        scanid = false;
                        ungetc(c, stdin);
                    }
                }
                match_keyword(token);
            case STATE_STRING_START:
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
        }
    }

    return ret;
}
