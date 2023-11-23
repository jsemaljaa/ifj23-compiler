#ifndef GENERATOR_H
#define GENERATOR_H

#include "scanner.h"
#include "parser.h"

void generatorPrintHead();

void generatorConvertBool();

void generatorStackPush(token_t token);

void generatorExpressionBegin();

void generatorExpressionEnd();

void generatorCheckTruth();

void generatorIfElse1(int number);

void generatorIfElse2(int number);

void generatorIfElse3(int number);

void generatorWhileLoop1(int number);

void generatorWhileLoop2(int number);

void generatorWhileLoop3(int number);

void generatorFindMath(); 

void generatorWrite(int countofparams);

void generatorStrLen();

void generatorOrd();

void generatorChr();

void generatorReadstring();

void generatorReadint();

void generatorReaddouble();

void generatorDoubleval();

void generatorIntval();

void generatorStrval();

void generatorSubstring();

void generatorFuncDef1(char *nameoffunction, int countofparams, parser_func_call_t *f)

void generatorFuncDef2(char *nameoffunction);

void generatorFuncCall(char *nameoffunction, int countofparams, datatype_t *functionReturnType)

void generatorReturn(char *nameoffunction, bool expr);

#endif