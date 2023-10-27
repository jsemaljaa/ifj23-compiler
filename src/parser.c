#include "parser.h"
#include "utils.h"

void ast_print(ast_node_t *node){
    if(node == NULL) return;
    ast_print(node->left);
    ast_print(node->mid);
    debug("-> %s", token_type_to_string(node->token.type));
    ast_print(node->right);
}

ast_node_t* parser_variable(token_t token) {
    ast_node_t *var_node = ast_create_leaf(token);
    get_token(&token);
    expect(token.type, TYPE_ID);
    var_node = ast_create_left(token, var_node);
    get_token(&token);
    expect(token.type, TYPE_ASSIGN);
    var_node = ast_create_left(token, var_node);
    get_token(&token);
    expect(token.type, TYPE_INT);
    var_node->right = ast_create_leaf(token);
    return var_node;
}

/*
 * @brief Parser entry function
 */
ast_node_t* parser_parse() {
    ast_node_t *program = NULL;
    ast_node_t *cursor = NULL;
    ast_node_t *sub_tree = NULL;

    token_create(token);
    
    get_token(&token);

    while (token.type != TYPE_EOF){
        debug("Token: %s", token_type_to_string(token.type));

        switch(token.type) {
            case TYPE_KW:
            {
                switch(token.attribute.keyword){
                    case K_VAR:
                    case K_LET:
                        sub_tree = parser_variable(token);
                        ast_print(sub_tree);
                        break;
                    default:
                    break;
                }
                break;
            }
            default:
                break;
        }

        if(sub_tree) {
            if(!program){
                program = ast_create_glue_left(sub_tree);
                cursor = program;
                get_token(&token);
                continue;
            }

            cursor->right = ast_create_glue_left(sub_tree);
            cursor = cursor->right;
        }

        get_token(&token);
    }

    return program;
}
