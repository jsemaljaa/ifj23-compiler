#include "parser.h"
#include "utils.h"

void expect_expression(token_t *token) {
   
    if (token->type != TYPE_INT && token->type != TYPE_STRING && token->type != TYPE_DOUBLE && token->type != TYPE_ID) {
        error("Unexpected token %s", token_type_to_string(token->type));
        exit(2);
    }
    get_token(token);

    while (token->type != TYPE_EOL) {
        expect_operator(token->type);
        get_token(token);

        if (token->type != TYPE_INT && token->type != TYPE_STRING && token->type != TYPE_DOUBLE && token->type != TYPE_ID) {
            error("Unexpected token %s", token_type_to_string(token->type));
            exit(2);
        }

        get_token(token);
    }
}

/*
 * @brief Parser entry function
 */
bool parser_parse(token_type_t endWhen, token_t *token) {

    get_token(token);

    while (token->type != endWhen){
        debug("Token: %s", token_type_to_string(token->type));
        // get_token(token);
        // continue;

        switch(token->type) {
            case TYPE_KW: //keyword
            {
                switch(token->attribute.keyword){
                    case K_VAR: //if variable definition
                    case K_LET:
                    {
                        get_token(token);
                        expect(token->type, TYPE_ID); //expect variable id

                        get_token(token);
                        switch(token->type) {
                            case TYPE_COLON: //if colon symbol
                            {
                                get_token(token);
                                expect(token->type, TYPE_KW); //expect keyword
                                get_token(token); //get token and fall through to next case
                            }

                            case TYPE_EOL:
                            case TYPE_EOF:
                                break; 

                            case TYPE_ASSIGN: //if assign symbol
                            {
                                get_token(token);
                                //expect data type
                                if (token->type != TYPE_INT && token->type != TYPE_STRING && token->type != TYPE_DOUBLE) {
                                    error("Unexpected token %s", token_type_to_string(token->type));
                                    exit(2);
                                }
                                break;
                            }
                            default:
                            break;
                        }
                        break;
                    }

                    case K_WHILE:
                    case K_IF:
                    {
                        keyword_t cycleType = token->attribute.keyword;
                        get_token(token);
                        //expect expression
                        expect_expression(token);
                        expect(token->type, TYPE_EOL);
                        get_token(token);
                        expect(token->type, TYPE_LBRACKET);
                        parser_parse(TYPE_RBRACKET, token);
                        get_token(token);
                        expect(token->type, TYPE_EOL);

                        if(cycleType == K_IF) {
                            get_token(token);
                            if (token->type == TYPE_KW && token->attribute.keyword == K_ELSE) {
                                get_token(token);
                                expect(token->type, TYPE_EOL);
                                get_token(token);
                                expect(token->type, TYPE_LBRACKET);
                                get_token(token);
                                expect(token->type, TYPE_EOL);
                                parser_parse(TYPE_RBRACKET, token);
                                get_token(token);
                                expect(token->type, TYPE_EOL);
                            }
                        }
                        break;
                    }

                    default:
                    break;
                }
                break;
            }

            case TYPE_ID: //identificator
            {
                get_token(token);
                expect(token->type, TYPE_ASSIGN);
                get_token(token);

                if (token->type != TYPE_INT && token->type != TYPE_STRING && token->type != TYPE_DOUBLE) {
                    error("Unexpected token %s", token_type_to_string(token->type));
                    exit(2);
                }
                break;
            }

            default:
                break;
        }


        get_token(token);
    }

    return 0;
}
