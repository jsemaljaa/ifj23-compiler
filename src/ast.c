#include "ast.h"

ast_node_t *ast_create_node(token_t token, ast_node_t *left, ast_node_t *mid, ast_node_t *right)
{
    ast_node_t *new_node = calloc(1, sizeof(ast_node_t));
    if(!new_node) return NULL;

    new_node->left = left;
    new_node->right = right;
    new_node->mid = mid;
    new_node->token = token;

    return new_node;
}

ast_node_t *ast_create_glue(ast_node_t *left, ast_node_t *mid, ast_node_t *right)
{
    token_t glue;
    glue.type = TYPE_GLUE;

    return ast_create_node(glue, left, mid, right);
}

ast_node_t *ast_create_glue_left(ast_node_t *left)
{
    token_t glue;
    glue.type = TYPE_GLUE;

    return ast_create_left(glue, left);
}

ast_node_t *ast_create_leaf(token_t token)
{
    return ast_create_node(token, NULL, NULL, NULL);
}

ast_node_t *ast_create_left(token_t token, ast_node_t *left)
{
    return ast_create_node(token, left, NULL, NULL);
}

void ast_destroy(ast_node_t *node)
{
    if(!node) return;

    ast_destroy(node->left);
    ast_destroy(node->mid);
    ast_destroy(node->right);

    free(node);
}
