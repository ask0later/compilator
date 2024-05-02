#ifndef IR_TO_OPCODESlib
#define IR_TO_OPCODESlib

#include "error_allocator.h"
#include "AST_to_IR.h"
#include "AST_to_nasm.h"




int IRtoOpcode(IR_Function* funcs, unsigned char* opcodes, size_t pos, err_allocator* err_alloc);
int PrintIRnodetoOpcode(IR_node* node, unsigned char* opcodes, size_t pos, err_allocator* err_alloc);
int TranslateIRtoOpcode(IR_node* node, err_allocator* err_alloc);


#endif