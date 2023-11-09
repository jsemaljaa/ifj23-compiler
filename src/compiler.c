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
    token_create(token);
    bool parser_result = parser_parse(TYPE_EOF, &token);

    // On error
    if(parser_result == 0){
        // TODO
        return -1;
    }

    return EXIT_SUCCESS;
}