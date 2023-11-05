#include "generator.h"

void genPrintHead()
{
    printf(".IFJcode23\n");
    printf("DEFVAR GF@%%nameoffunction\n");
    printf("MOVE GF@%%nameoffunction string@main\n");
    printf("DEFVAR GF@%%exprvalue\n");
    printf("MOVE GF@%%exprvalue nil@nil\n");
    printf("DEFVAR GF@%%current%%instruction\n");
    printf("MOVE GF@%%current%%instruction nil@nil\n");
    printf("DEFVAR GF@%%exists\n");

    printf("MOVE GF@%%exists nil@nil\n");       //mozna chyba???

    printf("DEFVAR GF@%%corrtype%%1\n");
    printf("MOVE GF@%%corrtype%%1 nil@nil\n");
    printf("DEFVAR GF@%%corrtype%%2\n");
    printf("MOVE GF@%%corrtype%%2 nil@nil\n");
    printf("DEFVAR GF@%%corrtype%%opt\n");
    printf("MOVE GF@%%corrtype%%opt nil@nil\n");
    printf("CREATEFRAME\n");
    printf("PUSHFRAME\n");
}

void genConvertBool()
{
    static int counter = 0;
    printf("CREATEFRAME\n");
    printf("PUSHFRAME\n");
    printf("DEFVAR LF@%%param1\n");
    printf("POPS LF@%%param1\n");
    printf("JUMPIFEQ %%bool%d%%false LF@%%param1 bool@false\n", counter);   //pokud je param1=false tak skoci na counter
    printf("MOVE LF@%%param1 int@1\n");
    printf("JUMP %%bool%d%%out\n", counter);
    printf("LABEL %%bool%d%%false\n", counter);
    printf("MOVE LF@%%param1 int@0\n");
    printf("LABEL %%bool%d%%out\n", counter);
    printf("PUSHS LF@%%param1\n");
    printf("POPFRAME\n");
    counter++;
}

void genStackPush(token_t token)
{
    switch (token.type)
    {

    case TYPE_INT:
        printf("PUSHS int@%d\n", token.attribute.integerNumber);
        break;

    case TYPE_DOUBLE:
        printf("PUSHS double@%a\n", token.attribute.floatNumber);
        break;

    case TYPE_KW:
            printf("PUSHS nil@nil\n");
            break;

    case TYPE_MUL:
        printf("MOVE GF@%%current%%instruction string@MULS\n");
        printf("CALL %%math%%check\n");
        printf("MULS\n");
        break;

    case TYPE_DIV:
        printf("MOVE GF@%%current%%instruction string@DIVS\n");
        printf("CALL %%math%%check\n");
        printf("DIVS\n");
        break;  

    case TYPE_MINUS:
        printf("MOVE GF@%%current%%instruction string@SUBS\n");
        printf("CALL %%math%%check\n");
        printf("SUBS\n");
        break;     

    case TYPE_PLUS:
        printf("MOVE GF@%%current%%instruction string@ADDS\n");
        printf("CALL %%math%%check\n");
        printf("ADDS\n");
        break;

    case TYPE_GT:
        printf("MOVE GF@%%current%%instruction string@GTS\n");
        printf("CALL %%math%%check\n");
        printf("GTS\n");
        genConvertBool();
        break;

    case TYPE_GE:
        printf("MOVE GF@%%current%%instruction string@GTE\n");
        printf("CALL %%math%%check\n");
        printf("GTS\n");
        printf("CREATEFRAME\n");
        printf("PUSHFRAME\n");
        printf("DEFVAR LF@GTE%%tmp\n");
        printf("POPS LF@GTE%%tmp\n");
        printf("EQS\n");
        printf("PUSHS LF@GTE%%tmp\n");
        printf("ORS\n");
        printf("POPFRAME\n");
        genConvertBool();
        break;

    case TYPE_LT:
        printf("MOVE GF@%%current%%instruction string@LTS\n");
        printf("CALL %%math%%check\n");
        printf("LTS\n");
        genConvertBool();
        break;

    case TYPE_LE:
        printf("MOVE GF@%%current%%instruction string@LTE\n");
        printf("CALL %%math%%check\n");
        printf("LTS\n");
        printf("CREATEFRAME\n");
        printf("PUSHFRAME\n");
        printf("DEFVAR LF@LTE%%tmp\n");
        printf("POPS LF@LTE%%tmp\n");
        printf("EQS\n");
        printf("PUSHS LF@LTE%%tmp\n");
        printf("ORS\n");
        printf("POPFRAME\n");
        genConvertBool();
        break;

    case TYPE_NEQ:
        printf("MOVE GF@%%current%%instruction string@EQS\n");
        printf("CALL %%math%%check\n");
        printf("EQS\n");
        printf("NOTS\n");
        genConvertBool();
        break;

    case TYPE_EQ:
        printf("MOVE GF@%%current%%instruction string@EQS\n");
        printf("CALL %%math%%check\n");
        printf("EQS\n");
        genConvertBool();
        break;
    }
}

void geneneratorExpressionBegin()
{
    printf("CREATEFRAME\n");
    printf("PUSHFRAME\n");
}

void geneneratorExpressionEnd()
{
    printf("POPS GF@%%exprvalue\n");
    printf("POPFRAME\n");
}

void geneneratorCheckTruth()
{
    printf("JUMP %%initialskip\n");
    printf("LABEL %%truthcheck\n");
    printf("CREATEFRAME\n");
    printf("PUSHFRAME\n");
    printf("DEFVAR LF@%%iftype\n");
    printf("TYPE LF@%%iftype GF@%%exprvalue\n");
    printf("DEFVAR LF@%%tmpbool\n");
    printf("JUMPIFEQ %%iftype%%int LF@%%iftype string@int\n");
    printf("JUMPIFEQ %%iftype%%float LF@%%iftype string@float\n");
    printf("JUMPIFEQ %%iftype%%nil LF@%%iftype string@nil\n");
    printf("JUMPIFEQ %%iftype%%string LF@%%iftype string@string\n");
    printf("EXIT int@7\n");
    printf("LABEL %%iftype%%int\n");
    printf("EQ LF@%%tmpbool GF@%%exprvalue int@0\n");
    printf("NOT LF@%%tmpbool LF@%%tmpbool\n");
    printf("MOVE GF@%%exprvalue LF@%%tmpbool\n");
    printf("JUMP %%out\n");
    printf("LABEL %%iftype%%float\n");
    printf("EQ LF@%%tmpbool GF@%%exprvalue float@0x0.0p+0\n");
    printf("NOT LF@%%tmpbool LF@%%tmpbool\n");
    printf("MOVE GF@%%exprvalue LF@%%tmpbool\n");
    printf("JUMP %%out\n");
    printf("LABEL %%iftype%%nil\n");
    printf("MOVE GF@%%exprvalue bool@false\n");
    printf("JUMP %%out\n");
    printf("LABEL %%iftype%%string\n");
    printf("JUMPIFEQ %%iftype%%string%%false GF@%%exprvalue string@0\n");
    printf("JUMPIFEQ %%iftype%%string%%false GF@%%exprvalue string@\n");
    printf("MOVE GF@%%exprvalue bool@true\n");
    printf("JUMP %%out\n");
    printf("LABEL %%iftype%%string%%false\n");
    printf("MOVE GF@%%exprvalue bool@false\n");
    printf("JUMP %%out\n");
    printf("LABEL %%out\n");
    printf("POPFRAME\n");
    printf("RETURN\n");
    printf("LABEL %%initialskip\n");
}

void geneneratorIfElse1(int number)
{
    printf("CALL %%truthcheck\n");
    printf("JUMPIFEQ if%%%d%%else GF@%%exprvalue bool@false\n", number);
}

void geneneratorIfElse2(int number)
{
    printf("JUMP if%%%d%%ifdefs\n", number);
    printf("LABEL if%%%d%%else\n", number);
}

void geneneratorIfElse3(int number)
{
    printf("LABEL if%%%d%%ifdefs\n", number);
    printf("JUMP if%%%d%%end\n", number);
    printf("LABEL if%%%d%%end\n", number);
}

void generatorWhileLoop1(int number)
{
    printf("LABEL while%%%d%%start\n", number);
}

void generatorWhileLoop2(int number)
{
    printf("CALL %%truthcheck\n");
    printf("JUMPIFEQ while%%%d%%end GF@%%exprvalue bool@false\n", number);
}

void generatorWhileLoop3(int number)
{
    printf("JUMP while%%%d%%start\n", number);
    printf("LABEL while%%%d%%end\n", number);
}

void geneneratorMathInstCheck()
{
    printf("JUMP %%skipcheck\n");
    printf("LABEL %%math%%check\n");
    printf("CREATEFRAME\n");
    printf("PUSHFRAME\n");
    printf("DEFVAR LF@tmp1\n");
    printf("DEFVAR LF@tmp1%%type\n");
    printf("POPS LF@tmp1\n");
    printf("TYPE LF@tmp1%%type LF@tmp1\n");
    printf("DEFVAR LF@tmp2\n");
    printf("DEFVAR LF@tmp2%%type\n");
    printf("POPS LF@tmp2\n");
    printf("TYPE LF@tmp2%%type LF@tmp2\n");
    printf("JUMPIFEQ %%ADDS%%SUBS%%MULS%%check GF@%%current%%instruction string@ADDS\n");
    printf("JUMPIFEQ %%ADDS%%SUBS%%MULS%%check GF@%%current%%instruction string@SUBS\n");
    printf("JUMPIFEQ %%ADDS%%SUBS%%MULS%%check GF@%%current%%instruction string@MULS\n");
    printf("JUMPIFEQ %%DIVS%%check GF@%%current%%instruction string@DIVS\n");
    printf("JUMPIFEQ %%CONCAT%%check GF@%%current%%instruction string@CONCAT\n");
    printf("JUMPIFEQ %%EQS%%check GF@%%current%%instruction string@EQS\n");
    printf("JUMPIFEQ %%LTS%%GTS%%check%%1 GF@%%current%%instruction string@GTS\n");
    printf("JUMPIFEQ %%LTS%%GTS%%check%%1 GF@%%current%%instruction string@LTS\n");
    printf("JUMPIFEQ %%LTS%%GTS%%check%%1 GF@%%current%%instruction string@GTE\n");
    printf("JUMPIFEQ %%LTS%%GTS%%check%%1 GF@%%current%%instruction string@LTE\n");
    printf("JUMP %%math%%check%%exit\n");
    // addition, subtraction, multiplication type check
    printf("LABEL %%ADDS%%SUBS%%MULS%%check\n");
    printf("JUMPIFEQ %%ADDS%%SUBS%%MULS%%check%%1 LF@tmp1%%type string@int\n");
    printf("JUMPIFEQ %%ADDS%%SUBS%%MULS%%check%%1 LF@tmp1%%type string@float\n");
    printf("JUMPIFEQ %%ADDS%%SUBS%%MULS%%check%%1 LF@tmp1%%type string@nil\n");
    printf("EXIT int@7\n");
    printf("LABEL %%ADDS%%SUBS%%MULS%%check%%1\n");
    printf("JUMPIFEQ %%ADDS%%SUBS%%MULS%%check%%2 LF@tmp2%%type string@int\n");
    printf("JUMPIFEQ %%ADDS%%SUBS%%MULS%%check%%2 LF@tmp2%%type string@float\n");
    printf("JUMPIFEQ %%ADDS%%SUBS%%MULS%%check%%2 LF@tmp2%%type string@nil\n");
    printf("EXIT int@7\n");
    printf("LABEL %%ADDS%%SUBS%%MULS%%check%%2\n");
    printf("TYPE LF@tmp1%%type LF@tmp1\n");
    printf("TYPE LF@tmp2%%type LF@tmp2\n");
    printf("JUMPIFEQ %%ADDS%%SUBS%%MULS%%check%%tmp1tozero LF@tmp1%%type string@nil\n");
    printf("JUMPIFEQ %%ADDS%%SUBS%%MULS%%check%%tmp2tozero LF@tmp2%%type string@nil\n");
    printf("JUMPIFEQ %%math%%check%%exit LF@tmp1%%type LF@tmp2%%type\n");
    printf("LABEL %%ADDS%%SUBS%%MULS%%check%%3\n");
    printf("JUMPIFEQ %%ADDS%%SUBS%%MULS%%check%%tmp1tofloat LF@tmp1%%type string@int\n");
    printf("JUMPIFEQ %%ADDS%%SUBS%%MULS%%check%%tmp2tofloat LF@tmp2%%type string@int\n");
    printf("JUMP %%math%%check%%exit\n");
    printf("LABEL %%ADDS%%SUBS%%MULS%%check%%tmp1tofloat\n");
    printf("INT2FLOAT LF@tmp1 LF@tmp1\n");
    printf("JUMP %%math%%check%%exit\n");
    printf("LABEL %%ADDS%%SUBS%%MULS%%check%%tmp2tofloat\n");
    printf("INT2FLOAT LF@tmp2 LF@tmp2\n");
    printf("JUMP %%math%%check%%exit\n");
    printf("LABEL %%ADDS%%SUBS%%MULS%%check%%tmp1tozero\n");
    printf("MOVE LF@tmp1 int@0\n");
    printf("JUMP %%ADDS%%SUBS%%MULS%%check%%2\n");
    printf("LABEL %%ADDS%%SUBS%%MULS%%check%%tmp2tozero\n");
    printf("MOVE LF@tmp2 int@0\n");
    printf("JUMP %%ADDS%%SUBS%%MULS%%check%%2\n");
    // division type check
    printf("LABEL %%DIVS%%check\n");
    printf("JUMPIFEQ %%DIVS%%check%%1 LF@tmp1%%type string@int\n");
    printf("JUMPIFEQ %%DIVS%%check%%1 LF@tmp1%%type string@float\n");
    printf("JUMPIFEQ %%DIVS%%check%%1 LF@tmp1%%type string@nil\n");
    printf("EXIT int@7\n");
    printf("LABEL %%DIVS%%check%%1\n");
    printf("JUMPIFEQ %%DIVS%%check%%2 LF@tmp2%%type string@int\n");
    printf("JUMPIFEQ %%DIVS%%check%%2 LF@tmp2%%type string@float\n");
    printf("JUMPIFEQ %%DIVS%%check%%2 LF@tmp2%%type string@nil\n");
    printf("EXIT int@7\n");
    printf("LABEL %%DIVS%%check%%2\n");
    printf("TYPE LF@tmp1%%type LF@tmp1\n");
    printf("TYPE LF@tmp2%%type LF@tmp2\n");
    printf("JUMPIFEQ %%DIVS%%check%%tmp1tozero LF@tmp1%%type string@nil\n");
    printf("JUMPIFEQ %%DIVS%%check%%tmp2tozero LF@tmp2%%type string@nil\n");
    printf("JUMPIFEQ %%DIVS%%check%%next LF@tmp1%%type string@float\n");
    printf("JUMP %%DIVS%%check%%tmp1tofloat\n");
    printf("LABEL %%DIVS%%check%%next\n");
    printf("JUMPIFEQ %%math%%check%%exit LF@tmp2%%type string@float\n");
    printf("JUMP %%DIVS%%check%%tmp2tofloat\n");
    printf("LABEL %%DIVS%%check%%tmp1tofloat\n");
    printf("INT2FLOAT LF@tmp1 LF@tmp1\n");
    printf("JUMP %%DIVS%%check%%2\n");
    printf("LABEL %%DIVS%%check%%tmp2tofloat\n");
    printf("INT2FLOAT LF@tmp2 LF@tmp2\n");
    printf("JUMP %%DIVS%%check%%2\n");
    printf("LABEL %%DIVS%%check%%tmp1tozero\n");
    printf("MOVE LF@tmp1 int@0\n");
    printf("JUMP %%DIVS%%check%%2\n");
    printf("LABEL %%DIVS%%check%%tmp2tozero\n");
    printf("MOVE LF@tmp2 int@0\n");
    printf("JUMP %%DIVS%%check%%2\n");
    // equals type check
    printf("LABEL %%EQS%%check\n");
    printf("JUMPIFEQ %%EQS%%check%%1 LF@tmp1%%type LF@tmp2%%type\n");
    printf("MOVE LF@tmp1 int@0\n");
    printf("MOVE LF@tmp2 int@1\n");
    printf("JUMP %%math%%check%%exit\n");
    printf("LABEL %%EQS%%check%%1\n");
    printf("JUMP %%math%%check%%exit\n");
    // lesser than & greater than type check
    printf("LABEL %%LTS%%GTS%%check%%1\n");
    printf("TYPE LF@tmp1%%type LF@tmp1\n");
    printf("TYPE LF@tmp2%%type LF@tmp2\n");
    printf("JUMPIFEQ %%LTS%%GTS%%check%%nil%%tmp1 LF@tmp1%%type nil@nil\n");
    printf("JUMPIFEQ %%LTS%%GTS%%check%%nil%%tmp2 LF@tmp2%%type nil@nil\n");
    printf("JUMP %%LTS%%GTS%%check%%2\n");
    printf("LABEL %%LTS%%GTS%%check%%nil%%tmp1\n");
    printf("JUMPIFEQ %%LTS%%GTS%%check%%tmp1tozero%%int LF@tmp2%%type string@nil\n");
    printf("JUMPIFEQ %%LTS%%GTS%%check%%tmp1tozero%%int LF@tmp2%%type string@int\n");
    printf("JUMPIFEQ %%LTS%%GTS%%check%%tmp1tozero%%float LF@tmp2%%type string@float\n");
    printf("JUMPIFEQ %%LTS%%GTS%%check%%tmp1toempty LF@tmp2%%type string@string\n");
    printf("JUMP %%LTS%%GTS%%check%%1\n");
    printf("LABEL %%LTS%%GTS%%check%%tmp1tozero%%int\n");
    printf("MOVE LF@tmp1 int@0\n");
    printf("JUMP %%LTS%%GTS%%check%%1\n");
    printf("LABEL %%LTS%%GTS%%check%%tmp1tozero%%float\n");
    printf("MOVE LF@tmp1 float@0x0.0p+0\n");
    printf("JUMP %%LTS%%GTS%%check%%1\n");
    printf("LABEL %%LTS%%GTS%%check%%tmp1toempty\n");
    printf("MOVE LF@tmp1 string@\n");
    printf("JUMP %%LTS%%GTS%%check%%1\n");
    printf("LABEL %%LTS%%GTS%%check%%nil%%tmp2\n");
    printf("JUMPIFEQ %%LTS%%GTS%%check%%tmp2tozero%%int LF@tmp1%%type string@int\n");
    printf("JUMPIFEQ %%LTS%%GTS%%check%%tmp2tozero%%float LF@tmp1%%type string@float\n");
    printf("JUMPIFEQ %%LTS%%GTS%%check%%tmp2toempty LF@tmp1%%type string@string\n");
    printf("JUMP %%LTS%%GTS%%check%%1\n");
    printf("LABEL %%LTS%%GTS%%check%%tmp2tozero%%int\n");
    printf("MOVE LF@tmp2 int@0\n");
    printf("JUMP %%LTS%%GTS%%check%%1\n");
    printf("LABEL %%LTS%%GTS%%check%%tmp2tozero%%float\n");
    printf("MOVE LF@tmp2 float@0x0.0p+0\n");
    printf("JUMP %%LTS%%GTS%%check%%1\n");
    printf("LABEL %%LTS%%GTS%%check%%tmp2toempty\n");
    printf("MOVE LF@tmp2 string@\n");
    printf("JUMP %%LTS%%GTS%%check%%1\n");
    printf("LABEL %%LTS%%GTS%%check%%2\n");
    printf("TYPE LF@tmp1%%type LF@tmp1\n");
    printf("TYPE LF@tmp2%%type LF@tmp2\n");
    printf("JUMPIFEQ %%LTS%%GTS%%check%%str LF@tmp1%%type string@string\n");
    printf("JUMPIFEQ %%LTS%%GTS%%check%%str LF@tmp2%%type string@string\n");
    printf("JUMPIFEQ %%LTS%%GTS%%check%%tmp1int LF@tmp1%%type string@int\n");
    printf("JUMPIFEQ %%LTS%%GTS%%check%%tmp2int LF@tmp2%%type string@int\n");
    printf("JUMPIFEQ %%math%%check%%exit LF@tmp1%%type LF@tmp2%%type\n");
    printf("JUMP %%math%%check%%exit\n");
    printf("LABEL %%LTS%%GTS%%check%%str\n");
    printf("JUMPIFEQ %%math%%check%%exit LF@tmp1%%type LF@tmp2%%type\n");
    printf("EXIT int@7\n");
    printf("LABEL %%LTS%%GTS%%check%%tmp1int\n");
    printf("JUMPIFEQ %%math%%check%%exit LF@tmp1%%type LF@tmp2%%type\n");
    printf("INT2FLOAT LF@tmp1 LF@tmp1\n");
    printf("JUMP %%LTS%%GTS%%check%%2\n");
    printf("LABEL %%LTS%%GTS%%check%%tmp2int\n");
    printf("INT2FLOAT LF@tmp2 LF@tmp2\n");
    printf("JUMP %%LTS%%GTS%%check%%2\n");
    // concatenation type check
    printf("LABEL %%CONCAT%%check\n");
    printf("JUMPIFEQ %%CONCAT%%check%%1 LF@tmp1%%type string@string\n");
    printf("JUMPIFEQ %%CONCAT%%check%%1 LF@tmp1%%type string@nil\n");
    printf("EXIT int@7\n");
    printf("LABEL %%CONCAT%%check%%1\n");
    printf("JUMPIFEQ %%CONCAT%%check%%2 LF@tmp2%%type string@string\n");
    printf("JUMPIFEQ %%CONCAT%%check%%2 LF@tmp2%%type string@nil\n");
    printf("EXIT int@7\n");
    printf("LABEL %%CONCAT%%check%%2\n");
    printf("TYPE LF@tmp1%%type LF@tmp1\n");
    printf("TYPE LF@tmp2%%type LF@tmp2\n");
    printf("JUMPIFEQ %%CONCAT%%check%%tmp1toempty LF@tmp1%%type string@nil\n");
    printf("JUMPIFEQ %%CONCAT%%check%%tmp2toempty LF@tmp2%%type string@nil\n");
    printf("JUMP %%math%%check%%exit\n");
    printf("LABEL %%CONCAT%%check%%tmp1toempty\n");
    printf("MOVE LF@tmp1 string@\n");
    printf("JUMP %%CONCAT%%check%%2\n");
    printf("LABEL %%CONCAT%%check%%tmp2toempty\n");
    printf("MOVE LF@tmp2 string@\n");
    printf("JUMP %%CONCAT%%check%%2\n");
    // end check for LTE & GTE operations
    printf("JUMPIFEQ %%OR%%EQUAL%%VARIANT GF@%%current%%instruction string@LTE\n");
    printf("JUMPIFEQ %%OR%%EQUAL%%VARIANT GF@%%current%%instruction string@GTE\n");
    printf("JUMP %%math%%check%%exit\n");
    printf("LABEL %%OR%%EQUAL%%VARIANT\n");
    printf("PUSHS LF@tmp2\n");
    printf("PUSHS LF@tmp1\n");
    printf("JUMP %%math%%check%%exit\n");
    // exit for all
    printf("LABEL %%math%%check%%exit\n");
    printf("PUSHS LF@tmp2\n");
    printf("PUSHS LF@tmp1\n");
    printf("POPFRAME\n");
    printf("RETURN\n");
    // type check for function parameters
    printf("LABEL %%corrtype%%func\n");
    printf("JUMPIFEQ %%match%%nil%%incl GF@%%corrtype%%opt int@1\n");
    printf("JUMPIFEQ %%match%%exact GF@%%corrtype%%opt int@0\n");
    printf("LABEL %%match%%nil%%incl\n");
    printf("JUMPIFEQ %%match%%passed GF@%%corrtype%%1 GF@%%corrtype%%2\n");
    printf("JUMPIFEQ %%match%%passed GF@%%corrtype%%1 string@nil\n");
    printf("EXIT int@4\n");
    printf("LABEL %%match%%exact\n");
    printf("JUMPIFEQ %%match%%passed GF@%%corrtype%%1 GF@%%corrtype%%2\n");
    printf("EXIT int@4\n");
    printf("LABEL %%match%%passed\n");
    printf("RETURN\n");
    printf("LABEL %%skipcheck\n");
}

void generateWrite(int countofparams)
{
    printf("CREATEFRAME\n");
    printf("PUSHFRAME\n");
    for (int i = countofparams; i > 0; i--)
    {
        printf("DEFVAR LF@%%write%d\n", i);
        printf("POPS LF@%%write%d\n", i);
    }
    for (int i = 1; i <= countofparams; i++)
    {
        printf("WRITE LF@%%write%d\n", i);
    }
    printf("POPFRAME\n");
}

void geneneratorStrLen()
{
    static int val = 0;
    printf("CREATEFRAME\n");
    printf("PUSHFRAME\n");
    printf("DEFVAR LF@str\n");
    printf("DEFVAR LF@type\n");
    printf("DEFVAR LF@strlen%%retval\n");
    printf("POPS LF@str\n");
    printf("TYPE LF@type LF@str\n");
    printf("JUMPIFEQ func%%strlen%%%d%%isstring LF@type string@string\n", val);
    printf("EXIT int@4\n");
    printf("LABEL func%%strlen%%%d%%isstring\n", val);
    printf("STRLEN LF@strlen%%retval LF@str\n");
    printf("POPFRAME\n");
    printf("MOVE GF@%%exprvalue TF@strlen%%retval\n");
    val++;
}

void geneneratorOrd() // ascii hodnota prvku v stringu
{
    static int val = 0;
    printf("CREATEFRAME\n");
    printf("PUSHFRAME\n");
    printf("DEFVAR LF@str\n");
    printf("DEFVAR LF@type\n");
    printf("DEFVAR LF@strlen%%retval\n");
    printf("POPS LF@str\n");
    printf("TYPE LF@type LF@str\n");
    printf("JUMPIFEQ func%%ord%%%d%%isstring LF@type string@string\n", val);
    printf("EXIT int@4\n");
    printf("LABEL func%%ord%%%d%%isstring\n", val);
    printf("STRLEN LF@strlen%%retval LF@str\n");
    printf("JUMPIFEQ func%%ord%%%d%%ordjump LF@strlen%%retval int@0\n", val);
    printf("STRI2INT LF@strlen%%retval LF@str int@0\n");
    printf("LABEL func%%ord%%%d%%ordjump\n", val);
    printf("POPFRAME\n");
    printf("MOVE GF@%%exprvalue TF@strlen%%retval\n");
    val++;
}

void geneneratorChr()
{
    static int val = 0;
    printf("CREATEFRAME\n");
    printf("PUSHFRAME\n");
    printf("DEFVAR LF@number\n");
    printf("DEFVAR LF@type\n");
    printf("DEFVAR LF@strlen%%retval\n");
    printf("MOVE LF@strlen%%retval string@a\n");
    printf("POPS LF@number\n");
    printf("TYPE LF@type LF@number\n");
    printf("JUMPIFEQ func%%chr%%%d%%isint LF@type string@int\n", val);
    printf("EXIT int@4\n");
    printf("LABEL func%%chr%%%d%%isint\n", val);
    printf("INT2CHAR LF@strlen%%retval LF@number\n");
    printf("POPFRAME\n");
    printf("MOVE GF@%%exprvalue TF@strlen%%retval\n");
    val++;
}

void generatorReadstring()
{
    printf("CREATEFRAME\n");
    printf("PUSHFRAME\n");
    printf("DEFVAR LF@Readstr%%retval\n");
    printf("READ LF@Readstr%%retval string\n");
    printf("POPFRAME\n");
    printf("MOVE GF@%%exprvalue TF@Readstr%%retval\n");
}

void generatorReadint()
{
    printf("CREATEFRAME\n");
    printf("PUSHFRAME\n");
    printf("DEFVAR LF@Readint%%retval\n");
    printf("READ LF@Readint%%retval int\n");
    printf("POPFRAME\n");
    printf("MOVE GF@%%exprvalue TF@Readint%%retval\n");
}

void generatorReaddouble()
{
    printf("CREATEFRAME\n");
    printf("PUSHFRAME\n");
    printf("DEFVAR LF@Readdouble%%retval\n");
    printf("READ LF@Readdouble%%retval double\n");
    printf("POPFRAME\n");
    printf("MOVE GF@%%exprvalue TF@Readdouble%%retval\n");
}

void generatorDoubleval()
{
    static int val = 0;
    printf("CREATEFRAME\n");
    printf("PUSHFRAME\n");
    printf("DEFVAR LF@%%doubleval%%%d%%retval\n", val);
    printf("DEFVAR LF@%%doubleval%%%d%%val\n", val);
    printf("DEFVAR LF@%%doubleval%%%d%%val%%type\n", val);
    printf("POPS LF@%%doubleval%%%d%%val\n", val);
    printf("TYPE LF@%%doubleval%%%d%%val%%type LF@%%doubleval%%%d%%val\n", val, val);
    printf("JUMPIFEQ %%func%%doubleval%%%d%%float LF@%%doubleval%%%d%%val%%type string@float\n", val, val);
    printf("JUMPIFEQ %%func%%doubleval%%%d%%int LF@%%doubleval%%%d%%val%%type string@int\n", val, val);
    printf("JUMPIFEQ %%func%%doubleval%%%d%%nil LF@%%doubleval%%%d%%val%%type string@nil\n", val, val);
    printf("JUMPIFEQ %%func%%doubleval%%%d%%string LF@%%doubleval%%%d%%val%%type string@string\n", val, val);
    printf("LABEL %%func%%doubleval%%%d%%float\n", val);
    printf("MOVE LF@%%doubleval%%%d%%retval LF@%%doubleval%%%d%%val\n", val, val);
    printf("JUMP %%func%%doubleval%%%d%%end\n", val);
    printf("LABEL %%func%%doubleval%%%d%%int\n", val);
    printf("INT2FLOAT LF@%%doubleval%%%d%%retval LF@%%doubleval%%%d%%val\n", val, val);
    printf("JUMP %%func%%doubleval%%%d%%end\n", val);
    printf("LABEL %%func%%doubleval%%%d%%string\n", val);
    printf("EXIT int@4\n");
    printf("LABEL %%func%%doubleval%%%d%%nil\n", val);
    printf("MOVE LF@%%doubleval%%%d%%retval float@0x0.0p+0\n", val);
    printf("JUMP %%func%%doubleval%%%d%%end\n", val);
    printf("LABEL %%func%%doubleval%%%d%%end\n", val);
    printf("POPFRAME\n");
    printf("MOVE GF@%%exprvalue TF@%%doubleval%%%d%%retval\n", val);
    val++;
}

void generatorIntval()
{
    static int val = 0;
    printf("CREATEFRAME\n");
    printf("PUSHFRAME\n");
    printf("DEFVAR LF@%%intval%%%d%%retval\n", val);
    printf("DEFVAR LF@%%intval%%%d%%val\n", val);
    printf("DEFVAR LF@%%intval%%%d%%val%%type\n", val);
    printf("POPS LF@%%intval%%%d%%val\n", val);
    printf("TYPE LF@%%intval%%%d%%val%%type LF@%%intval%%%d%%val\n", val, val);
    printf("JUMPIFEQ %%func%%intval%%%d%%int LF@%%intval%%%d%%val%%type string@int\n", val, val);
    printf("JUMPIFEQ %%func%%intval%%%d%%float LF@%%intval%%%d%%val%%type string@float\n", val, val);
    printf("JUMPIFEQ %%func%%intval%%%d%%nil LF@%%intval%%%d%%val%%type string@nil\n", val, val);
    printf("JUMPIFEQ %%func%%intval%%%d%%string LF@%%intval%%%d%%val%%type string@string\n", val, val);
    printf("LABEL %%func%%intval%%%d%%int\n", val);
    printf("MOVE LF@%%intval%%%d%%retval LF@%%intval%%%d%%val\n", val, val);
    printf("JUMP %%func%%intval%%%d%%end\n", val);
    printf("LABEL %%func%%intval%%%d%%float\n", val);
    printf("FLOAT2INT LF@%%intval%%%d%%retval LF@%%intval%%%d%%val\n", val, val);
    printf("JUMP %%func%%intval%%%d%%end\n", val);
    printf("LABEL %%func%%intval%%%d%%nil\n", val);
    printf("MOVE LF@%%intval%%%d%%retval int@0\n", val);
    printf("JUMP %%func%%intval%%%d%%end\n", val);
    printf("LABEL %%func%%intval%%%d%%string\n", val);
    printf("EXIT int@4\n");
    printf("LABEL %%func%%intval%%%d%%end\n", val);
    printf("POPFRAME\n");
    printf("MOVE GF@%%exprvalue TF@%%intval%%%d%%retval\n", val);
    val++;
}

void generatorStrval()
{
    static int val = 0;
    printf("CREATEFRAME\n");
    printf("PUSHFRAME\n");
    printf("DEFVAR LF@%%stringval%%%d%%retval\n", val);
    printf("DEFVAR LF@%%stringval%%%d%%val\n", val);
    printf("DEFVAR LF@%%stringval%%%d%%val%%type\n", val);
    printf("POPS LF@%%stringval%%%d%%val\n", val);
    printf("TYPE LF@%%stringval%%%d%%val%%type LF@%%stringval%%%d%%val\n", val, val);
    printf("JUMPIFEQ %%func%%stringval%%%d%%string LF@%%stringval%%%d%%val%%type string@string\n", val, val);
    printf("JUMPIFEQ %%func%%stringval%%%d%%int LF@%%stringval%%%d%%val%%type string@int\n", val, val);
    printf("JUMPIFEQ %%func%%stringval%%%d%%float LF@%%stringval%%%d%%val%%type string@float\n", val, val);
    printf("JUMPIFEQ %%func%%stringval%%%d%%nil LF@%%stringval%%%d%%val%%type string@nil\n", val, val);
    printf("LABEL %%func%%stringval%%%d%%string\n", val);
    printf("MOVE LF@%%stringval%%%d%%retval LF@%%stringval%%%d%%val\n", val, val);
    printf("JUMP %%func%%stringval%%%d%%end\n", val);
    printf("LABEL %%func%%stringval%%%d%%nil\n", val);
    printf("MOVE LF@%%stringval%%%d%%retval string@\n", val);
    printf("JUMP %%func%%stringval%%%d%%end\n", val);
    printf("LABEL %%func%%stringval%%%d%%int\n", val);
    printf("EXIT int@4\n");
    printf("LABEL %%func%%stringval%%%d%%float\n", val);
    printf("EXIT int@4\n");
    printf("LABEL %%func%%stringval%%%d%%end\n", val);
    printf("POPFRAME\n");
    printf("MOVE GF@%%exprvalue TF@%%stringval%%%d%%retval\n", val);
    val++;
}

void generatorSubstring()
{
    static int val = 0;
    printf("CREATEFRAME\n");
    printf("PUSHFRAME\n");
    printf("DEFVAR LF@%%substring%%%d%%retval\n", val);
    printf("DEFVAR LF@%%substring%%%d%%currchr\n", val);
    printf("DEFVAR LF@%%i\n");
    printf("DEFVAR LF@%%j\n");
    printf("DEFVAR LF@%%str\n");
    printf("DEFVAR LF@%%throwaway\n");
    printf("DEFVAR LF@%%len\n");
    printf("MOVE LF@%%substring%%%d%%retval nil@nil\n", val);
    printf("POPS LF@%%j\n");
    printf("POPS LF@%%i\n");
    printf("POPS LF@%%str\n");
    printf("TYPE LF@%%throwaway LF@%%j\n");
    printf("JUMPIFNEQ %%badtype%d LF@%%throwaway string@int\n", val);
    printf("TYPE LF@%%throwaway LF@%%i\n");
    printf("JUMPIFNEQ %%badtype%d LF@%%throwaway string@int\n", val);
    printf("TYPE LF@%%throwaway LF@%%str\n");
    printf("JUMPIFNEQ %%badtype%d LF@%%throwaway string@string\n", val);
    printf("JUMP %%correcttype%d\n", val);
    printf("LABEL %%badtype%d\n", val);
    printf("EXIT int@4\n");
    printf("LABEL %%correcttype%d\n", val);
    printf("GT LF@%%throwaway LF@%%i LF@%%j\n");
    printf("JUMPIFEQ %%returnnull%d LF@%%throwaway bool@true\n", val);
    printf("LT LF@%%throwaway LF@%%i int@0\n");
    printf("JUMPIFEQ %%returnnull%d LF@%%throwaway bool@true\n", val);
    printf("LT LF@%%throwaway LF@%%j int@0\n");
    printf("JUMPIFEQ %%returnnull%d LF@%%throwaway bool@true\n", val);
    printf("STRLEN LF@%%throwaway LF@%%str\n");
    printf("EQ LF@%%throwaway LF@%%throwaway LF@%%i\n");
    printf("JUMPIFEQ %%returnnull%d LF@%%throwaway bool@true\n", val);
    printf("STRLEN LF@%%throwaway LF@%%str\n");
    printf("GT LF@%%throwaway LF@%%i LF@%%throwaway\n");
    printf("JUMPIFEQ %%returnnull%d LF@%%throwaway bool@true\n", val);
    printf("STRLEN LF@%%throwaway LF@%%str\n");
    printf("GT LF@%%throwaway LF@%%j LF@%%throwaway\n");
    printf("JUMPIFEQ %%returnnull%d LF@%%throwaway bool@true\n", val);

    printf("MOVE LF@%%len LF@%%j\n");
    printf("SUB LF@%%len LF@%%len LF@%%i\n");
    printf("MOVE LF@%%substring%%%d%%retval string@\n", val);
    printf("JUMPIFEQ %%returnnull%d LF@%%len int@0\n", val);
    printf("LABEL %%for%d\n", val);
    printf("GETCHAR LF@%%substring%%%d%%currchr LF@%%str LF@%%i\n", val);
    printf("CONCAT LF@%%substring%%%d%%retval LF@%%substring%%%d%%retval LF@%%substring%%%d%%currchr\n", val, val, val);
    printf("SUB LF@%%len LF@%%len int@1\n");
    printf("ADD LF@%%i LF@%%i int@1\n");
    printf("JUMPIFNEQ %%for%d LF@%%len int@0\n", val);
    printf("LABEL %%returnnull%d\n", val);
    printf("POPFRAME\n");
    printf("MOVE GF@%%exprvalue TF@%%substring%%%d%%retval\n", val);
    val++;
}

void geneneratorFuncDef1(char *nameoffunction, int parCount, ast_node_t *ast)
{

    printf("JUMP %%jump%%over%%%s\n", nameoffunction);
    printf("LABEL %%func%%%s\n", nameoffunction);
    printf("CREATEFRAME\n");
    printf("DEFVAR TF@%s%%retval\n", nameoffunction);
    token_t tmp = ast->token;
    for (size_t i = parCount; i > 0; i--)
    {
        printf("DEFVAR TF@%s\n", tmp.attribute.id);
        printf("POPS TF@%s\n", tmp.attribute.id);
        tmp = ast->right->token;
    }
    tmp = ast->token;
    for (size_t i = parCount; i > 0; i--)
    {
        char *matchingType = "";
        if (tmp.type == "TYPE_DOUBLE")
        {
            matchingType = "double";
        }
        else if (tmp.type == "TYPE_INT")
        {
            matchingType = "int";
        }
        else if (tmp.type == "TYPE_STRING")
        {
            matchingType = "string";
        }
        printf("TYPE GF@%%corrtype%%1 TF@%s\n", tmp.attribute.id);
        printf("MOVE GF@%%corrtype%%2 string@%s\n", matchingType);
        printf("MOVE GF@%%corrtype%%opt int@%d\n", tmp.attribute.integerNumber);
        printf("CALL %%corrtype%%func\n");
        tmp = ast->left->token;
    }
    printf("PUSHFRAME\n");
}

void generatorFuncDef2(char *nameoffunction)
{
    printf("POPFRAME\n");
    printf("RETURN\n");
    printf("LABEL %%jump%%over%%%s\n", nameoffunction);
}

void generatorFuncCall(char *nameoffunction, int paramCount, token_t *returnType)
{
    static int returnnum = 0;
    if (strcmp(nameoffunction, "write") == 0)
    {
        generateWrite(paramCount);
    }
    else if (strcmp(nameoffunction, "strlen") == 0)
    {
        geneneratorStrLen();
    }
    else if (strcmp(nameoffunction, "ord") == 0)
    {
        geneneratorOrd();
    }
    else if (strcmp(nameoffunction, "chr") == 0)
    {
        geneneratorChr();
    }
    else if (strcmp(nameoffunction, "Readstr") == 0)
    {
        generatorReadstring();
    }
    else if (strcmp(nameoffunction, "Readint") == 0)
    {
        generatorReadint();
    }
    else if (strcmp(nameoffunction, "Readdouble") == 0)
    {
        generatorReaddouble();
    }
    else if (strcmp(nameoffunction, "doubleval") == 0)
    {
        generatorDoubleval();
    }
    else if (strcmp(nameoffunction, "intval") == 0)
    {
        generatorIntval();
    }
    else if (strcmp(nameoffunction, "stringval") == 0)
    {
        generatorStrval();
    }
    else if (strcmp(nameoffunction, "substring") == 0)
    {
        generatorSubstring();
    }
    else
    {
        printf("CALL %%func%%%s\n", nameoffunction);
        if (returnType->type != K_NONE)
        {
            printf("TYPE GF@%%corrtype%%1 TF@%s%%retval\n", nameoffunction);
            printf("JUMPIFEQ %%hasnil%d GF@%%corrtype%%1 string@nil\n", returnnum);
            printf("JUMPIFNEQ %%hasretvalue%d GF@%%corrtype%%1 string@\n", returnnum);
            printf("LABEL %%hasnil%d\n", returnnum);
            if (returnType->attribute.integerNumber == '1')
            {
                printf("JUMPIFEQ %%hasretvalue%d GF@%%corrtype%%1 string@nil\n", returnnum);
            }
            printf("EXIT int@4\n");
            printf("LABEL %%hasretvalue%d\n", returnnum);
            printf("MOVE GF@%%exprvalue TF@%s%%retval\n", nameoffunction);
        }
    }
    returnnum++;
}

void generatorReturn(char *nameoffunction, bool expr)
{
    (void)expr;
    if (strcmp(nameoffunction, "main") != 0)
    {
        printf("MOVE LF@%s%%retval GF@%%exprvalue\n", nameoffunction);
        printf("POPFRAME\n");
        printf("RETURN\n");
    }
    else
    {
        printf("EXIT int@0\n");
    }
}