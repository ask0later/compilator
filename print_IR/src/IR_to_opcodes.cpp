#include "IR_to_opcodes.h"
#include "AST_to_IR.h"
#include "error_allocator.h"
#include <cstring>


int IRtoOpcode(IR_Function* funcs, unsigned char* opcodes, size_t pos, err_allocator* err_alloc)
{

        size_t func_num = 0;
        for (; func_num < MAX_FUNC_NUM && funcs[func_num].size != 0; func_num++)
                ;
        
        for (size_t func_index = 0; func_index < func_num; func_index++)
        {
                for (size_t instr_index = 0; instr_index < funcs[func_index].size; instr_index++)
                {
                        IR_Function* func = funcs + func_index;
                        PrintIRnodetoOpcode(func->instrs + instr_index, opcodes, pos, err_alloc);                        
                }
        }

        return 0;
}

int PrintIRnodetoOpcode(IR_node* node, unsigned char* opcodes, size_t pos, err_allocator* err_alloc)
{
        //TranslateIRtoOpcode(node, err_alloc);

        size_t instr_size = strlen((char*) node->x64_instr);
        memcpy(opcodes + pos, node->x64_instr, instr_size);
        pos += instr_size;

        return 0;
}