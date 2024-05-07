#ifndef IR_TO_NASMlib
#define IR_TO_NASMlib

#include "error_allocator.h"
#include "AST_to_IR.h"


const char* const temp_registers[] = {"r11", "r12", "r13", "r14"};

const char* const nasm_instrs[] = {"nop",
                                   "mov",
                                   "add",
                                   "sub",
                                   "mul",
                                   "div",
                                   "push",
                                   "pop",
                                   "jmp",
                                   "je",
                                   "jne",
                                   "jae",
                                   "jbe",
                                   "cmp",
                                   "",
                                   "call",
                                   "ret",
                                   "",
                                   "",
                                   "",
                                   "syscall"};

int PrintIRtoNasm(FILE* To, IR_Function* funcs);

int PrintIRFunction(FILE* To, IR_Function* funcs);
int PrintIRInstruction(FILE* To, IR_block* ir_block);
int PrintIRArgument(FILE* To, IR_type type, IR_data* data);

int PrintInOutLib(FILE* To);

#endif