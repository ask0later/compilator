#include "IR_to_nasm.h"



int PrintIRtoNasm(IR_node* ir_nodes, size_t index_node, err_allocator* err_alloc)
{
        for (size_t i = 0; i < index_node; i++)
        {
                ParseInstruction(ir_nodes[i]);
        }

        return 0;
}

int ParseInstruction(IR_node ir_nodes)
{

        switch(ir_nodes.instr)
        {
        case IR_NOP:

                break;
        case IR_MOV: 
                
                break;
        case IR_ADD: 
                
                break;
        case IR_SUB: 
                
                break;
        case IR_MUL: 
                
                break;
        case IR_DIV: 
                
                break;
        case IR_PUSH: 
                
                break;
        case IR_POP: 
                
                break;
        case IR_JMP: 
                
                break;
        case IR_JE: 
                
                break;
        case IR_JNE: 
                
                break;
        case IR_JAE: 
                
                break;
        case IR_JBE: 
                
                break;
        case IR_CMP: 
                
                break;
        case IR_DEFINE: 
                
                break;
        case IR_CALL: 
                
                break;
        case IR_RET: 
                
                break;
        case IR_INPUT: 
                
                break;
        case IR_OUTPUT: 
                
                break;
        case IR_LABEL: 
                
                break;








        
        }


        return 0;
}