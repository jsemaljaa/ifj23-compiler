/*
 * generator.h
 *
 * @brief Declarations for structures and functions for generator
 * @author Nazar Neskoromnyi <xnesko00@vutbr.cz>
 */

#ifndef GEN_H
#define GEN_H

//#include <symtable.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
    
}InstrType;

typedef struct Instruction {
    int op_cnt;
    int prm_cnt;
    InstrType instruct;
    char *id;
    //htab_data_t **operands;
    DataType *types; //musi byt nekdy definice typov like none, int, string
    //htab_data_t **params;
} Instruction;

typedef struct Generator {
    int lbl_cnt;
    Instruction **instructions;
    int instr_cnt;
    //TStack *lbl_stack; zeptat o stacke
} Generator;

void gen_init(Generator restrict *gen){
    gen->lbl_cnt = 0;
    gen->instructions = malloc(sizeof(Instruction *) * /*x*/)
    if(gen->instructions == NULL)
        Error();
    gen->instr_cnt = 0;
    //nejaky stack init
}

void gen_init(Generator *);

void add_instruction(Generator *, Instruction *);

void gen_free(Generator *);

Instruction *constructor(InstructionType instruct, char *id, htab_data_t **operands, DataType *types, int op_cnt, htab_data_t **params, int prm_cnt);


#endif