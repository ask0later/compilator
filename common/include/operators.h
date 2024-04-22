#ifndef OPERlib
#define OPERlib

#include "tree.h"

const size_t SIZE_SYMBOLS      = 10;
const size_t SIZE_MORSE_SYMBOL = 10;

struct Command
{
    char    name[MAX_SIZE_ARG];
    size_t           size_name;
    Operators               id;
};


#define OP(KIND, LANGUAGE_PRINT, IR_PRINT) \
{IR_PRINT, sizeof(IR_PRINT) - 1, KIND} ,   \

const Command operators[] = 
{
    #include "../../codegen/commands.h"
};

#undef OP


#define OP(KIND, LANGUAGE_PRINT, IR_PRINT)          \
{LANGUAGE_PRINT, sizeof(LANGUAGE_PRINT) - 1, KIND}, \

const Command language_cmds[] = 
{
    #include "../../codegen/commands.h"
};

#undef OP


const Command unary_cmds[] = 
   {{"sin",  sizeof("sin") - 1 ,    FUN_SIN  },
    {"cos",  sizeof("cos") - 1 ,    FUN_COS  },
    {"sqrt", sizeof("sqrt") - 1,    FUN_SQRT },
    {"ln",   sizeof("ln") - 1  ,    FUN_LN   },
    {"-",    sizeof("-") - 1   ,    OP_UN_SUB},
    {"out",  sizeof("out") - 1 ,    RET      }};

#endif