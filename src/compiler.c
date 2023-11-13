/*
 * compiler.c
 *
 * @brief Main module
 * @author Alina Vinogradova <xvinog00@vutbr.cz>
 */

#include "parser.h"

int main(){
    int err = parse();

    // On error
    printf("Exit with code: %d\n", err);
    return err;
}