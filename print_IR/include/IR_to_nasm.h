#ifndef IR_TO_NASMlib
#define IR_TO_NASMlib

#include "error_allocator.h"
#include "print_IR.h"


int PrintIRtoNasm(IR_node* ir_nodes, size_t* index_node, err_allocator* err_alloc);

int ParseInstruction(IR_node ir_nodes);






#endif