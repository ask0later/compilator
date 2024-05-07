#ifndef IR_TO_OPCODESlib
#define IR_TO_OPCODESlib

#include "error_allocator.h"
#include "AST_to_IR.h"
#include "AST_to_nasm.h"


const int ALIGNMENT = 0x1000;

struct Segments
{
        Text    text;
        Text    data;
};


int IRtoOpcode(IR_Function* funcs, Text* code, err_allocator* err_alloc);
int TranslateInstrToOpcode(IR_block* ir_block, Text* code);

int AddLib(Text* code, err_allocator* err_alloc);


#endif