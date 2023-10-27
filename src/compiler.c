/*
 * compiler.c
 *
 * @brief Main module
 * @author Alina Vinogradova <xvinog00@vutbr.cz>
 */

#include "scanner.h"
#include "utils.h"
#include "parser.h"

int main(){
    ast_node_t *ast = parser_parse();

    // On error
    if(!ast){
        // TODO
        return -1;
    }

    return EXIT_SUCCESS;
}