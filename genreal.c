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
    printf("DEFVAR GF@%%corrtype%%1\n");
    printf("MOVE GF@%%corrtype%%1 nil@nil\n");
    printf("DEFVAR GF@%%corrtype%%2\n");
    printf("MOVE GF@%%corrtype%%2 nil@nil\n");
    printf("DEFVAR GF@%%corrtype%%opt\n");
    printf("MOVE GF@%%corrtype%%opt nil@nil\n");
    printf("CREATEFRAME\n");
    printf("PUSHFRAME\n");
}

void generatorConvertBool()
{
    static int counter = 0;
    printf("CREATEFRAME\n");
    printf("PUSHFRAME\n");
    printf("DEFVAR LF@%%paramforconvert\n");
    printf("POPS LF@%%paramforconvert\n");
    printf("JUMPIFEQ %%false%d%%val LF@%%paramforconvert bool@false\n", counter);
    printf("MOVE LF@%%paramforconvert int@1\n");
    printf("JUMP %%finish%%bool\n", counter);
    printf("LABEL %%false%d%%val\n", counter);
    printf("MOVE LF@%%paramforconvert int@0\n");
    printf("LABEL %%finish%%bool\n", counter);
    printf("PUSHS LF@%%paramforconvert\n");
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

    case TYPE_STRING:
    {
        string_t temp;
        str_create(&temp , STR_SIZE );
        int i = 0, j = 0, val = 0;
        char esc[5] = "";
        while ((val = token.value.string.s[i]) != '\0')
        {
            if (val < 33 || val == 35 || val == 92 || val > 126)
            {
                str_append(&temp, '\\');
                if (val < 0)
                    val += 256;
                sprintf(esc, "%03d", val);
                for (j = 0; j < 3; j++)
                {
                    str_append(&temp, esc[j]);
                }
            }
            else
            {
                str_append(&temp, val);
            }
            i++;
        }
        printf("PUSHS string@%s\n", temp.s);
        str_free(&temp);
        str_free(&token.value.string);
        break;
    }
    case TYPE_PLUS:
        printf("MOVE GF@%%current%%instruction string@ADDS\n");
        printf("CALL %%find%%math\n");
        printf("ADDS\n");
        break;

    case TYPE_MINUS:
        printf("MOVE GF@%%current%%instruction string@SUBS\n");
        printf("CALL %%find%%math\n");
        printf("SUBS\n");
        break;   
        
    case TYPE_MUL:
        printf("MOVE GF@%%current%%instruction string@MULS\n");
        printf("CALL %%find%%math\n");
        printf("MULS\n");
        break;   

    case TYPE_DIV:
        printf("MOVE GF@%%current%%instruction string@DIVS\n");
        printf("CALL %%find%%math\n");
        printf("DIVS\n");
        break;         

    case TYPE_KW:
            printf("PUSHS nil@nil\n");
            break;

    case TYPE_GT:
        printf("MOVE GF@%%current%%instruction string@GTS\n");
        printf("CALL %%find%%math\n");
        printf("GTS\n");
        genConvertBool();
        break;

    case TYPE_GE:
        printf("MOVE GF@%%current%%instruction string@GTE\n");
        printf("CALL %%find%%math\n");
        printf("GTS\n");
        printf("CREATEFRAME\n");
        printf("PUSHFRAME\n");
        printf("DEFVAR LF@GTE%%VAL\n");
        printf("POPS LF@GTE%%VAL\n");
        printf("EQS\n");
        printf("PUSHS LF@GTE%%VAL\n");
        printf("ORS\n");
        printf("POPFRAME\n");
        genConvertBool();
        break;

    case TYPE_LT:
        printf("MOVE GF@%%current%%instruction string@LTS\n");
        printf("CALL %%find%%math\n");
        printf("LTS\n");
        genConvertBool();
        break;

    case TYPE_LE:
        printf("MOVE GF@%%current%%instruction string@LTE\n");
        printf("CALL %%find%%math\n");
        printf("LTS\n");
        printf("CREATEFRAME\n");
        printf("PUSHFRAME\n");
        printf("DEFVAR LF@LTE%%VAL\n");
        printf("POPS LF@LTE%%VAL\n");
        printf("EQS\n");
        printf("PUSHS LF@LTE%%VAL\n");
        printf("ORS\n");
        printf("POPFRAME\n");
        genConvertBool();
        break;

    case TYPE_NEQ:
        printf("MOVE GF@%%current%%instruction string@EQS\n");
        printf("CALL %%find%%math\n");
        printf("EQS\n");
        printf("NOTS\n");
        genConvertBool();
        break;

    case TYPE_EQ:
        printf("MOVE GF@%%current%%instruction string@EQS\n");
        printf("CALL %%find%%math\n");
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
    printf("DEFVAR LF@%%boolVAL\n");
    printf("JUMPIFEQ %%iftype%%int LF@%%iftype string@int\n");
    printf("JUMPIFEQ %%iftype%%float LF@%%iftype string@float\n");
    printf("JUMPIFEQ %%iftype%%nil LF@%%iftype string@nil\n");
    printf("JUMPIFEQ %%iftype%%string LF@%%iftype string@string\n");
    printf("EXIT int@7\n");
    printf("LABEL %%iftype%%int\n");
    printf("EQ LF@%%boolVAL GF@%%exprvalue int@0\n");
    printf("NOT LF@%%boolVAL LF@%%boolVAL\n");
    printf("MOVE GF@%%exprvalue LF@%%boolVAL\n");
    printf("JUMP %%out\n");
    printf("LABEL %%iftype%%float\n");
    printf("EQ LF@%%boolVAL GF@%%exprvalue float@0x0.0p+0\n");
    printf("NOT LF@%%boolVAL LF@%%boolVAL\n");
    printf("MOVE GF@%%exprvalue LF@%%boolVAL\n");
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

void geneneratorFindMath()
{
    printf("JUMP %%skipcheck\n");
    printf("LABEL %%find%%math\n");
    printf("CREATEFRAME\n");
    printf("PUSHFRAME\n");
    printf("DEFVAR LF@val1\n");
    printf("DEFVAR LF@val1%%type\n");
    printf("POPS LF@val1\n");
    printf("TYPE LF@val1%%type LF@val1\n");
    printf("DEFVAR LF@val2\n");
    printf("DEFVAR LF@val2%%type\n");
    printf("POPS LF@val2\n");
    printf("TYPE LF@val2%%type LF@val2\n");
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
    printf("JUMP %%find%%math%%exit\n");
    printf("LABEL %%ADDS%%SUBS%%MULS%%check\n");
    printf("JUMPIFEQ %%ADDS%%SUBS%%MULS%%check%%1 LF@val1%%type string@int\n");
    printf("JUMPIFEQ %%ADDS%%SUBS%%MULS%%check%%1 LF@val1%%type string@float\n");
    printf("JUMPIFEQ %%ADDS%%SUBS%%MULS%%check%%1 LF@val1%%type string@nil\n");
    printf("EXIT int@7\n");
    printf("LABEL %%ADDS%%SUBS%%MULS%%check%%1\n");
    printf("JUMPIFEQ %%ADDS%%SUBS%%MULS%%check%%2 LF@val2%%type string@int\n");
    printf("JUMPIFEQ %%ADDS%%SUBS%%MULS%%check%%2 LF@val2%%type string@float\n");
    printf("JUMPIFEQ %%ADDS%%SUBS%%MULS%%check%%2 LF@val2%%type string@nil\n");
    printf("EXIT int@7\n");
    printf("LABEL %%ADDS%%SUBS%%MULS%%check%%2\n");
    printf("TYPE LF@val1%%type LF@val1\n");
    printf("TYPE LF@val2%%type LF@val2\n");
    printf("JUMPIFEQ %%ADDS%%SUBS%%MULS%%check%%val1tozero LF@val1%%type string@nil\n");
    printf("JUMPIFEQ %%ADDS%%SUBS%%MULS%%check%%val2tozero LF@val2%%type string@nil\n");
    printf("JUMPIFEQ %%find%%math%%exit LF@val1%%type LF@val2%%type\n");
    printf("LABEL %%ADDS%%SUBS%%MULS%%check%%3\n");
    printf("JUMPIFEQ %%ADDS%%SUBS%%MULS%%check%%val1tofloat LF@val1%%type string@int\n");
    printf("JUMPIFEQ %%ADDS%%SUBS%%MULS%%check%%val2tofloat LF@val2%%type string@int\n");
    printf("JUMP %%find%%math%%exit\n");
    printf("LABEL %%ADDS%%SUBS%%MULS%%check%%val1tofloat\n");
    printf("INT2FLOAT LF@val1 LF@val1\n");
    printf("JUMP %%find%%math%%exit\n");
    printf("LABEL %%ADDS%%SUBS%%MULS%%check%%val2tofloat\n");
    printf("INT2FLOAT LF@val2 LF@val2\n");
    printf("JUMP %%find%%math%%exit\n");
    printf("LABEL %%ADDS%%SUBS%%MULS%%check%%val1tozero\n");
    printf("MOVE LF@val1 int@0\n");
    printf("JUMP %%ADDS%%SUBS%%MULS%%check%%2\n");
    printf("LABEL %%ADDS%%SUBS%%MULS%%check%%val2tozero\n");
    printf("MOVE LF@val2 int@0\n");
    printf("JUMP %%ADDS%%SUBS%%MULS%%check%%2\n");
    printf("LABEL %%DIVS%%check\n");
    printf("JUMPIFEQ %%DIVS%%check%%1 LF@val1%%type string@int\n");
    printf("JUMPIFEQ %%DIVS%%check%%1 LF@val1%%type string@float\n");
    printf("JUMPIFEQ %%DIVS%%check%%1 LF@val1%%type string@nil\n");
    printf("EXIT int@7\n");
    printf("LABEL %%DIVS%%check%%1\n");
    printf("JUMPIFEQ %%DIVS%%check%%2 LF@val2%%type string@int\n");
    printf("JUMPIFEQ %%DIVS%%check%%2 LF@val2%%type string@float\n");
    printf("JUMPIFEQ %%DIVS%%check%%2 LF@val2%%type string@nil\n");
    printf("EXIT int@7\n");
    printf("LABEL %%DIVS%%check%%2\n");
    printf("TYPE LF@val1%%type LF@val1\n");
    printf("TYPE LF@val2%%type LF@val2\n");
    printf("JUMPIFEQ %%DIVS%%check%%val1tozero LF@val1%%type string@nil\n");
    printf("JUMPIFEQ %%DIVS%%check%%val2tozero LF@val2%%type string@nil\n");
    printf("JUMPIFEQ %%DIVS%%check%%next LF@val1%%type string@float\n");
    printf("JUMP %%DIVS%%check%%val1tofloat\n");
    printf("LABEL %%DIVS%%check%%next\n");
    printf("JUMPIFEQ %%find%%math%%exit LF@val2%%type string@float\n");
    printf("JUMP %%DIVS%%check%%val2tofloat\n");
    printf("LABEL %%DIVS%%check%%val1tofloat\n");
    printf("INT2FLOAT LF@val1 LF@val1\n");
    printf("JUMP %%DIVS%%check%%2\n");
    printf("LABEL %%DIVS%%check%%val2tofloat\n");
    printf("INT2FLOAT LF@val2 LF@val2\n");
    printf("JUMP %%DIVS%%check%%2\n");
    printf("LABEL %%DIVS%%check%%val1tozero\n");
    printf("MOVE LF@val1 int@0\n");
    printf("JUMP %%DIVS%%check%%2\n");
    printf("LABEL %%DIVS%%check%%val2tozero\n");
    printf("MOVE LF@val2 int@0\n");
    printf("JUMP %%DIVS%%check%%2\n");
    printf("LABEL %%EQS%%check\n");
    printf("JUMPIFEQ %%EQS%%check%%1 LF@val1%%type LF@val2%%type\n");
    printf("MOVE LF@val1 int@0\n");
    printf("MOVE LF@val2 int@1\n");
    printf("JUMP %%find%%math%%exit\n");
    printf("LABEL %%EQS%%check%%1\n");
    printf("JUMP %%find%%math%%exit\n");
    printf("LABEL %%LTS%%GTS%%check%%1\n");
    printf("TYPE LF@val1%%type LF@val1\n");
    printf("TYPE LF@val2%%type LF@val2\n");
    printf("JUMPIFEQ %%LTS%%GTS%%check%%nil%%val1 LF@val1%%type nil@nil\n");
    printf("JUMPIFEQ %%LTS%%GTS%%check%%nil%%val2 LF@val2%%type nil@nil\n");
    printf("JUMP %%LTS%%GTS%%check%%2\n");
    printf("LABEL %%LTS%%GTS%%check%%nil%%val1\n");
    printf("JUMPIFEQ %%LTS%%GTS%%check%%val1tozero%%int LF@val2%%type string@nil\n");
    printf("JUMPIFEQ %%LTS%%GTS%%check%%val1tozero%%int LF@val2%%type string@int\n");
    printf("JUMPIFEQ %%LTS%%GTS%%check%%val1tozero%%float LF@val2%%type string@float\n");
    printf("JUMPIFEQ %%LTS%%GTS%%check%%val1toempty LF@val2%%type string@string\n");
    printf("JUMP %%LTS%%GTS%%check%%1\n");
    printf("LABEL %%LTS%%GTS%%check%%val1tozero%%int\n");
    printf("MOVE LF@val1 int@0\n");
    printf("JUMP %%LTS%%GTS%%check%%1\n");
    printf("LABEL %%LTS%%GTS%%check%%val1tozero%%float\n");
    printf("MOVE LF@val1 float@0x0.0p+0\n");
    printf("JUMP %%LTS%%GTS%%check%%1\n");
    printf("LABEL %%LTS%%GTS%%check%%val1toempty\n");
    printf("MOVE LF@val1 string@\n");
    printf("JUMP %%LTS%%GTS%%check%%1\n");
    printf("LABEL %%LTS%%GTS%%check%%nil%%val2\n");
    printf("JUMPIFEQ %%LTS%%GTS%%check%%val2tozero%%int LF@val1%%type string@int\n");
    printf("JUMPIFEQ %%LTS%%GTS%%check%%val2tozero%%float LF@val1%%type string@float\n");
    printf("JUMPIFEQ %%LTS%%GTS%%check%%val2toempty LF@val1%%type string@string\n");
    printf("JUMP %%LTS%%GTS%%check%%1\n");
    printf("LABEL %%LTS%%GTS%%check%%val2tozero%%int\n");
    printf("MOVE LF@val2 int@0\n");
    printf("JUMP %%LTS%%GTS%%check%%1\n");
    printf("LABEL %%LTS%%GTS%%check%%val2tozero%%float\n");
    printf("MOVE LF@val2 float@0x0.0p+0\n");
    printf("JUMP %%LTS%%GTS%%check%%1\n");
    printf("LABEL %%LTS%%GTS%%check%%val2toempty\n");
    printf("MOVE LF@val2 string@\n");
    printf("JUMP %%LTS%%GTS%%check%%1\n");
    printf("LABEL %%LTS%%GTS%%check%%2\n");
    printf("TYPE LF@val1%%type LF@val1\n");
    printf("TYPE LF@val2%%type LF@val2\n");
    printf("JUMPIFEQ %%LTS%%GTS%%check%%str LF@val1%%type string@string\n");
    printf("JUMPIFEQ %%LTS%%GTS%%check%%str LF@val2%%type string@string\n");
    printf("JUMPIFEQ %%LTS%%GTS%%check%%val1int LF@val1%%type string@int\n");
    printf("JUMPIFEQ %%LTS%%GTS%%check%%val2int LF@val2%%type string@int\n");
    printf("JUMPIFEQ %%find%%math%%exit LF@val1%%type LF@val2%%type\n");
    printf("JUMP %%find%%math%%exit\n");
    printf("LABEL %%LTS%%GTS%%check%%str\n");
    printf("JUMPIFEQ %%find%%math%%exit LF@val1%%type LF@val2%%type\n");
    printf("EXIT int@7\n");
    printf("LABEL %%LTS%%GTS%%check%%val1int\n");
    printf("JUMPIFEQ %%find%%math%%exit LF@val1%%type LF@val2%%type\n");
    printf("INT2FLOAT LF@val1 LF@val1\n");
    printf("JUMP %%LTS%%GTS%%check%%2\n");
    printf("LABEL %%LTS%%GTS%%check%%val2int\n");
    printf("INT2FLOAT LF@val2 LF@val2\n");
    printf("JUMP %%LTS%%GTS%%check%%2\n");
    printf("LABEL %%CONCAT%%check\n");
    printf("JUMPIFEQ %%CONCAT%%check%%1 LF@val1%%type string@string\n");
    printf("JUMPIFEQ %%CONCAT%%check%%1 LF@val1%%type string@nil\n");
    printf("EXIT int@7\n");
    printf("LABEL %%CONCAT%%check%%1\n");
    printf("JUMPIFEQ %%CONCAT%%check%%2 LF@val2%%type string@string\n");
    printf("JUMPIFEQ %%CONCAT%%check%%2 LF@val2%%type string@nil\n");
    printf("EXIT int@7\n");
    printf("LABEL %%CONCAT%%check%%2\n");
    printf("TYPE LF@val1%%type LF@val1\n");
    printf("TYPE LF@val2%%type LF@val2\n");
    printf("JUMPIFEQ %%CONCAT%%check%%val1toempty LF@val1%%type string@nil\n");
    printf("JUMPIFEQ %%CONCAT%%check%%val2toempty LF@val2%%type string@nil\n");
    printf("JUMP %%find%%math%%exit\n");
    printf("LABEL %%CONCAT%%check%%val1toempty\n");
    printf("MOVE LF@val1 string@\n");
    printf("JUMP %%CONCAT%%check%%2\n");
    printf("LABEL %%CONCAT%%check%%val2toempty\n");
    printf("MOVE LF@val2 string@\n");
    printf("JUMP %%CONCAT%%check%%2\n");
    printf("JUMPIFEQ %%OR%%EQUAL%%VARIANT GF@%%current%%instruction string@LTE\n");
    printf("JUMPIFEQ %%OR%%EQUAL%%VARIANT GF@%%current%%instruction string@GTE\n");
    printf("JUMP %%find%%math%%exit\n");
    printf("LABEL %%OR%%EQUAL%%VARIANT\n");
    printf("PUSHS LF@val2\n");
    printf("PUSHS LF@val1\n");
    printf("JUMP %%find%%math%%exit\n");
    printf("LABEL %%find%%math%%exit\n");
    printf("PUSHS LF@val2\n");
    printf("PUSHS LF@val1\n");
    printf("POPFRAME\n");
    printf("RETURN\n");
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

void geneneratorOrd()
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

void geneneratorFuncDef1(char *nameoffunction, int countofparams, parser_func_call_t *f)
{

    printf("JUMP %%jump%%over%%%s\n", nameoffunction);
    printf("LABEL %%func%%%s\n", nameoffunction);
    printf("CREATEFRAME\n");
    printf("DEFVAR TF@%s%%retval\n", nameoffunction);
    param_t val = f->params;
    for (int i = countofparams; i > 0; i--)
    {
        printf("DEFVAR TF@%s\n", val.attribute.id);
        printf("POPS TF@%s\n", val.attribute.id);
        val = f->right->params;
    }
    val = f->params;
    for (int i = countofparams; i > 0; i--)
    {
        char *matchingType = "";
        if (val.type == "TYPE_DOUBLE")
        {
            matchingType = "double";
        }
        else if (val.type == "TYPE_INT")
        {
            matchingType = "int";
        }
        else if (val.type == "TYPE_STRING")
        {
            matchingType = "string";
        }
        printf("TYPE GF@%%corrtype%%1 TF@%s\n", val.attribute.id);
        printf("MOVE GF@%%corrtype%%2 string@%s\n", matchingType);
        printf("MOVE GF@%%corrtype%%opt int@%d\n", val.attribute.integerNumber);
        printf("CALL %%corrtype%%func\n");
        val = f->left->params;
    }
    printf("PUSHFRAME\n");
}

void generatorFuncDef2(char *nameoffunction)
{
    printf("POPFRAME\n");
    printf("RETURN\n");
    printf("LABEL %%jump%%over%%%s\n", nameoffunction);
}

void generatorFuncCall(char *nameoffunction, int countofparams, datatype_t *functionReturnType)
{
    static int returnnum = 0;
    if (strcmp(nameoffunction, "Readint") == 0)
    {
        generatorReadint();
    }
    else if (strcmp(nameoffunction, "Readdouble") == 0)
    {
        generatorReaddouble();
    }
    else if (strcmp(nameoffunction, "Readstr") == 0)
    {
        generatorReadstring();
    }
    else if (strcmp(nameoffunction, "intval") == 0)
    {
        generatorIntval();
    }
    else if (strcmp(nameoffunction, "doubleval") == 0)
    {
        generatorDoubleval();
    }   
    else if (strcmp(nameoffunction, "stringval") == 0)
    {
        generatorStrval();
    }
    else if (strcmp(nameoffunction, "write") == 0)
    {
        generateWrite(countofparams);
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
    else if (strcmp(nameoffunction, "substring") == 0)
    {
        generatorSubstring();
    }
    else
    {
        printf("CALL %%func%%%s\n", nameoffunction);
        if (functionReturnType->keyword_t != K_NONE)
        {
            printf("TYPE GF@%%corrtype%%1 TF@%s%%retval\n", nameoffunction);
            printf("JUMPIFEQ %%hasnil%d GF@%%corrtype%%1 string@nil\n", returnnum);
            printf("JUMPIFNEQ %%hasretvalue%d GF@%%corrtype%%1 string@\n", returnnum);
            printf("LABEL %%hasnil%d\n", returnnum);
            if (functionReturnType->attribute.integerNumber == '1')
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