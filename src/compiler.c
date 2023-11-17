/*
 * Project: IFJ23 compiler
 *
 * compiler.c
 *
 * @brief Main module
 *
 * @author Alina Vinogradova <xvinog00@vutbr.cz>
 */

#include "parser.h"

int main(){
    int err = parse();


    // On error
    printf("Exit: %s\n", get_text_code(err));
    return err;
}