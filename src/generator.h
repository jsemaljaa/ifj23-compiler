#ifndef GENERATOR_H
#define GENERATOR_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ast.h>    
#include <symtable.h>
#include <error.h>
#include <scanner.h>
#include <parser.h>

#define INITIAL_SIZE 128

void geneneratorPrintHead();

void generatorConvertBool();

void geneneratorStackPush(token_t token);

void geneneratorExpressionBegin();

void geneneratorExpressionEnd();

void geneneratorCheckTruth();

void generatorIfElse1(int number);

void generatorIfElse2(int number);

void generatorIfElse3(int number);

void generatorWhileLoop1(int number);

void generatorWhileLoop2(int number);

void generatorWhileLoop3(int number);

void geneneratorMathInstCheck(); 

void generateWrite(int countofparams);

void geneneratorStrLen();

void geneneratorOrd();

void geneneratorChr();

void generatorReadstring();

void generatorReadint();

void generatorReaddouble();

void generatorDoubleval();

void generatorIntval();

void generatorStrval();

void generatorSubstring();

void geneneratorFuncDef1(char *nameoffunction, int parCount, ast_node_t *ast);

void generatorFuncDef2(char *nameoffunction);

void generatorFuncCall(char *nameoffunction, int paramCount, token_t *returnType);

void generatorReturn(char *nameoffunction, bool expr);

#endif
