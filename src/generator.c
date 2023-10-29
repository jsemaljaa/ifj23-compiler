/*
 * generator.c
 *
 * @brief Implementation of generator
 * @author Nazar Neskoromnyi <xnesko00@vutbr.cz>
 */

#include <generator.h>
#include <stdio.h>

void gen_free(register Generator *gen) {
    for (int i = 0; i < gen->instr_cnt; i++) {
        if (gen->instructions[i]->operands != NULL) {
            free(gen->instructions[i]->operands);
        }
        gen->instructions[i]->operands = NULL;

        if (gen->instructions[i]->params != NULL) {
            free(gen->instructions[i]->params);
        }
        gen->instructions[i]->params = NULL;

        free(gen->instructions[i]);
        gen->instructions[i] = NULL;

    }
    free(gen->instructions);
    gen->instructions = NULL;
    //stack_free(gen->lbl_stack);
}

Instruction *constructor(InstructionType instruct, char *id, /*htab_data_t **operands,*/ DataType *types, int op_cnt,/* htab_data_t **params,*/ int prm_cnt) 
{
    Instruction *instruction = malloc(sizeof(Instruction));
    instruction->instruct = instruct;
    instruction->id = id;
    instruction->operands = operands;
    instruction->types = types;
    instruction->op_cnt = op_cnt;
    instruction->params = params;
    instruction->prm_cnt = prm_cnt;
    return instruction;
}

void add_instruction(Generator * restrict gen, Instruction *instruction) {
    if (gen->instr_cnt % 100 == 0) {
        gen->instructions = realloc(gen->instructions, sizeof(Instruction*) * (gen->instr_cnt + 300));
        if (generator->instructions == NULL)
            Error();
    }
    gen->instructions[gen->instr_cnt] = instruction;
    gen->instr_cnt++;
}