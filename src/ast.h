#ifndef AST_H
#define AST_H

#include "scanner.h"
/*
 * @brief Structure representing AST node
 */
typedef struct ast_node {
    token_t token;
    struct ast_node *left, *mid, *right;
} ast_node_t;

ast_node_t *ast_create_node(token_t token, 
                                ast_node_t *left,
                                ast_node_t *mid,
                                ast_node_t *right);

ast_node_t *ast_create_glue(ast_node_t *left,
                                ast_node_t *mid,
                                ast_node_t *right);

ast_node_t *ast_create_glue_left(ast_node_t *left);

ast_node_t *ast_create_leaf(token_t token);

ast_node_t *ast_create_left(token_t token, ast_node_t *left);

void ast_destroy(ast_node_t *node);

#endif // AST_H