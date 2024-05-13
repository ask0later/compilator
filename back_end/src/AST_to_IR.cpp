#include "AST_to_IR.h"
#include "error_allocator.h"
#include "read_file.h"
#include "tree.h"
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>


int CtorIR(IR_Function** ir_funcs, err_allocator* err_alloc)
{
        *ir_funcs = (IR_Function*) calloc(MAX_FUNC_NUM, sizeof(IR_Function));

        if (!(*ir_funcs))
        {
                INSERT_ERROR_NODE(err_alloc, "dynamic allocation is fault");
                err_alloc->need_call = true;
                return 1;
        }

        for (size_t i = 0; i < MAX_FUNC_NUM; i++)
        {
                (*ir_funcs)[i].size = 0;
                (*ir_funcs)[i].if_num = 0;
                (*ir_funcs)[i].while_num = 0;
                (*ir_funcs)[i].position = 0;
                
                (*ir_funcs)[i].instrs = (IR_block*) calloc(MAX_IR_BLOCK, sizeof(IR_block));
                
                if (!((*ir_funcs)[i].instrs))
                {
                        INSERT_ERROR_NODE(err_alloc, "dynamic allocation is fault");
                        err_alloc->need_call = true;
                        return 1;
                }
        }

        return 0;
}


void DtorIR(IR_Function* ir_funcs)
{
        for (size_t i = 0; i < MAX_FUNC_NUM; i++)
        {
                free(ir_funcs[i].instrs);
        }

        free(ir_funcs);
}

int InsertIRblock(IR_Function* ir_func, IR_Instruction instr_id, IR_type type_1, int arg_1, char* name_1, IR_type type_2, int arg_2, char* name_2)
{
        IR_block* ir_block = ir_func->instrs + ir_func->position;

        ir_block->instr  = instr_id;

        ir_block->type_1 = type_1;
        ir_block->arg_1.value = arg_1;

        if (name_1)
                memcpy(ir_block->arg_1.name, name_1, strlen(name_1));

        ir_block->type_2 = type_2;
        ir_block->arg_2.value = arg_2;

        if (name_2)
                memcpy(ir_block->arg_2.name, name_2, strlen(name_2));

        InsertOpcode(ir_block);

        if (ir_func->position)
                ir_block->offset += (ir_block - 1)->x64_instr_size + (ir_block - 1)->offset;
        else
                ir_block->offset = 0;

        return 0;
}

int InsertOpcode(IR_block* ir_block)
{
        bool match = true;
        for (size_t instr_index = 0; instr_index < sizeof(table) / sizeof(IR_to_opcode); instr_index++)
        {
                if (ir_block->instr == table[instr_index].ir_instr)
                {
                        const args_and_opcode* data_table = (const args_and_opcode*) table[instr_index].data;

                        for (size_t data_index = 0; data_index < sizeof(table[instr_index].data) / sizeof(args_and_opcode); data_index++)
                        {
                                match = true;
                                
                                match &= data_table[data_index].type_1 == ir_block->type_1;                                
                                match &= data_table[data_index].type_2 == ir_block->type_2;
                                
                                if (ir_block->type_1 != NUM_ARG && ir_block->type_1 != LABEL_ARG && ir_block->type_1 != MEM_ARG)
                                        match &= data_table[data_index].arg_1 == ir_block->arg_1.value;
                                
                                if (ir_block->type_2 != NUM_ARG && ir_block->type_2 != MEM_ARG)
                                        match &= data_table[data_index].arg_2  == ir_block->arg_2.value;

                                if (match)
                                {
                                        ir_block->x64_instr_size = data_table[data_index].opcode_size;
                                        memcpy(ir_block->x64_instr, data_table[data_index].opcode, ir_block->x64_instr_size);
                                        break;
                                }
                        }
                        break;
                }
        }


        if (ir_block->type_1 == NUM_ARG || ir_block->type_1 == MEM_ARG)
        {
                int dec_number = ir_block->arg_1.value;
                unsigned char hex_number[sizeof(int)] = {};
                
                ConvertToHex(dec_number, (unsigned char*) hex_number);

                size_t offset = ir_block->x64_instr_size - sizeof(int);
                memcpy(ir_block->x64_instr + offset, hex_number, sizeof(int));
        }

        if (ir_block->type_2 == NUM_ARG)
        {
                int dec_number = ir_block->arg_2.value;
                unsigned char hex_number[sizeof(int)] = {};
                
                ConvertToHex(dec_number, (unsigned char*) hex_number);

                size_t offset = ir_block->x64_instr_size - sizeof(int);
                memcpy(ir_block->x64_instr + offset, hex_number, sizeof(int));
        }

        return 0;
}

int ConvertToHex(int dec_number, unsigned char* hex_number)
{
        for (size_t digit_index = 0; digit_index < sizeof(int); digit_index++)
        {       
                hex_number[digit_index] = (unsigned char) (dec_number & 0xFF);
                dec_number = dec_number >> 8;
        }

        return 0;
}

int ConvertHexToBuf(char* hex_buf, unsigned char* hex_number, size_t bytes)
{
        unsigned char symbol_1 = 0, symbol_2 = 0;

        for (size_t byte_index = 0; byte_index < bytes; byte_index++)
        {
                symbol_1 = (hex_number[byte_index] & 0xF0);
                symbol_1 = symbol_1 >> 4;
                symbol_2 = (hex_number[byte_index] & 0x0F);

                if (symbol_1 < 0x0A)
                        hex_buf[2 * byte_index] = (char) (symbol_1 + '0');
                else
                        hex_buf[2 * byte_index] = (char) (symbol_1 + 'A' - 0x0A);

                if (symbol_2 < 0x0A)
                        hex_buf[2 * byte_index + 1] = (char) (symbol_2 + '0');
                else
                        hex_buf[2 * byte_index + 1] = (char) (symbol_2 + 'A' - 0x0A);
        }

        return 0;
}

int PatchIR(IR_Function* ir_funcs, err_allocator* err_alloc)
{
        Label func_labels[MAX_FUNC_NUM] = {};

        size_t func_num = 0;

        for (; func_num < MAX_FUNC_NUM && ir_funcs[func_num].size != 0; func_num++)
        {
                Label* start_label = ir_funcs[func_num].label;

                strcpy(func_labels[func_num].label_name, start_label->label_name);
                func_labels[func_num].offset = start_label->offset + ir_funcs[func_num].offset_from_start;
        }

        IR_Function* last_func = ir_funcs + func_num - 1;

        Label* in_label  = func_labels + func_num;
        Label* out_label = func_labels + func_num + 1;
        func_num += 2;
        
        memcpy(in_label->label_name, "my_input", sizeof("my_input"));
        in_label->offset = last_func->offset_from_start + last_func->bytes;
     

        memcpy(out_label->label_name, "my_output", sizeof("my_output"));
        out_label->offset = in_label->offset + MY_INPUT_SIZE;

        for (size_t func_index = 0; func_index < func_num; func_index++)
        {
                PatchJumps(ir_funcs + func_index, func_labels, func_num, err_alloc);
        }

        return 0;
}

int PatchJumps(IR_Function* func, Label* func_labels, size_t func_num, err_allocator* err_alloc)
{
        for (size_t instr_index = 0; instr_index < func->size; instr_index++)
        {
                IR_block* ir_block = func->instrs + instr_index;
                
                size_t offset = 0;
                offset += GetOffsetConditionalJumps(ir_block, func, err_alloc);
                offset += GetOffsetGlobalCalls(ir_block, func_labels, func_num, func->offset_from_start, err_alloc);

                if (err_alloc->need_call)
                {
                        INSERT_ERROR_NODE(err_alloc, "unable to determine offset");
                }

                if (offset != 0)
                {
                        unsigned char hex_number[sizeof(int)] = {};
                        ConvertToHex((int) offset, hex_number);
                        size_t offset_index = ir_block->x64_instr_size - sizeof(int);

                        memcpy(ir_block->x64_instr + offset_index, hex_number, sizeof(int));
                }
        }

        return 0;
}

size_t GetOffsetConditionalJumps(IR_block* ir_block, IR_Function* func, err_allocator* err_alloc)
{
        if (ir_block->instr == IR_JE || ir_block->instr == IR_JNE || ir_block->instr == IR_JAE || ir_block->instr == IR_JBE || ir_block->instr == IR_JMP)
        {
                for (size_t label_index = 0; label_index < func->label_num; label_index++)
                {
                        if (strcmp(func->label[label_index].label_name, ir_block->arg_1.name) == 0)
                        {
                                size_t offset = func->label[label_index].offset - (ir_block->offset + ir_block->x64_instr_size);
                                return offset;
                        }
                }
                
                INSERT_ERROR_NODE(err_alloc, "label not declared");
                err_alloc->need_call = true;
        }

        return 0;
}

size_t GetOffsetGlobalCalls(IR_block* ir_block, Label* func_labels, size_t func_num, size_t func_offset, err_allocator* err_alloc)
{
        if (ir_block->instr == IR_CALL)
        {
                for (size_t func_index = 0; func_index < func_num; func_index++)
                {
                        if (strcmp(ir_block->arg_1.name, func_labels[func_index].label_name) == 0)
                        {
                                size_t offset = func_labels[func_index].offset - (ir_block->offset + ir_block->x64_instr_size + func_offset);
                                return offset;
                        }
                }
                
                INSERT_ERROR_NODE(err_alloc, "label not declared");
                err_alloc->need_call = true;
        }

        return 0;
}


int CompleteProgramExit(IR_Function* func)
{
        InsertIRblock(func, IR_PUSH, NUM_ARG, 60, NULL, NO_ARG, 0, NULL);
        func->position++;

        char rax[] = "rax";
        char rdi[] = "rdi";

        InsertIRblock(func, IR_POP, REG_ARG, 0, rax, NO_ARG, 0, NULL);
        func->position++;

        InsertIRblock(func, IR_PUSH, NUM_ARG, 0, NULL, NO_ARG, 0, NULL);
        func->position++;

        InsertIRblock(func, IR_POP, REG_ARG, 7, rdi, NO_ARG, 0, NULL);
        func->position++;

        InsertIRblock(func, IR_SYSCALL, NO_ARG, 0, NULL, NO_ARG, 0, NULL);
        func->position++;

        return 0;
}


int ConvertIR(IR_Function* ir_funcs, Tree** trees, err_allocator* err_alloc)
{
        size_t func_num = 0;
        for (;func_num < NUM_TREE && trees[func_num] != NULL; func_num++)
                ;

        func_num--;
                
        IR_Function* main = ir_funcs;

        char r10[] = "r10";
        char rsp[] = "rsp";

        CompleteFunctionIR(ir_funcs, trees[func_num]->root, err_alloc);
        if (err_alloc->need_call == true)
        {
                INSERT_ERROR_NODE(err_alloc, "invalid executing CompleteFunctionIR");
                return 1;
        }

        InsertIRblock(main, IR_MOV, REG_ARG, 4, rsp, REG_ARG, 10, r10);
        main->position++;

        CompleteProgramExit(main);
        
        size_t pos = main->position;
        main->size = pos;

        IR_block* last = main->instrs + pos - 1;

        main->bytes = last->offset + last->x64_instr_size;
        main->position = 0;

        size_t offset_from_start = main->bytes;


        for (size_t func_index = 0; func_index < func_num; func_index++)
        {
                IR_Function* func = ir_funcs + func_index + 1;

                CompleteFunctionIR(func, trees[func_index]->root, err_alloc);
                if (err_alloc->need_call == true)
                {
                        INSERT_ERROR_NODE(err_alloc, "invalid executing CompleteFunctionIR");
                        return 1;
                }

                pos = func->position;
                last = func->instrs + pos - 1;

                func->bytes = last->offset + last->x64_instr_size;

                func->offset_from_start = offset_from_start;
                func->size = pos;
                func->position = 0;

                offset_from_start += func->bytes;
        }

        PatchIR(ir_funcs, err_alloc);

        return 0;
}

int CompleteFunctionIR(IR_Function* ir_func, Node* node, err_allocator* err_alloc)
{
        if (node->type == FUNCTION)
        {
                char r10[] = "r10";
                char rsp[] = "rsp";

                char buf[BUFFER_SIZE] = {};
                int id_fun = (int) node->data.id_fun;
                sprintf(buf, "func_%d", id_fun);

                InsertIRblock(ir_func, IR_DEFINE, LABEL_ARG, id_fun, buf, NO_ARG, 0, NULL);
                
                memcpy(ir_func->label[ir_func->label_num].label_name, buf, strlen(buf));
                ir_func->label[ir_func->label_num].offset = ir_func->instrs[ir_func->position].offset;
                ir_func->label_num++;

                ir_func->position++;

                InsertIRblock(ir_func, IR_MOV, REG_ARG, 10, r10, REG_ARG, 4, rsp);
                ir_func->position++;

                InsertIRblock(ir_func, IR_SUB, REG_ARG, 10, r10, NUM_ARG, 8, NULL);
                ir_func->position++;


                if (!node->right)
                {
                        CompleteOperatorsIR(ir_func, node->left, err_alloc);
                }
                else
                {       
                        CompletePopArgumentsIR(ir_func, node->left, err_alloc);
                        CompleteOperatorsIR(ir_func, node->right, err_alloc);
                }
        }
        
        return 0;
}

int CompletePopArgumentsIR(IR_Function* ir_func, Node* node, err_allocator* err_alloc)
{
        if (!node) return 0;

        if (node->type != VARIABLE)
        {
                CompletePopArgumentsIR(ir_func, node->right, err_alloc);
                CompletePopArgumentsIR(ir_func, node->left, err_alloc);
        }
        else if (node->type == VARIABLE)
        {
                char rsp[] = "rsp";

                InsertIRblock(ir_func, IR_SUB, REG_ARG, 4, rsp, NUM_ARG, 8, NULL);
                ir_func->position++;
        }

        return 0;
}

int CompletePushArgumentsIR(IR_Function* ir_func, Node* node, int* arg_num, err_allocator* err_alloc)
{
        if (!node) return 0;

        bool is_comma = false;

        if (node->type == OPERATOR)
                if (node->data.id_op == COMMA)
                        is_comma = true;

        if (is_comma)
        {
                CompletePushArgumentsIR(ir_func, node->right, arg_num, err_alloc);
                CompletePushArgumentsIR(ir_func, node->left, arg_num, err_alloc);
        }
        else if (!is_comma)
        {
                CompleteExpressionIR(ir_func, node, err_alloc);
                (*arg_num)++;
        }

        return 0;
}

void CompleteOperatorsIR(IR_Function* ir_func, Node* node, err_allocator* err_alloc)
{
        if (!node) {return;}

        if (node->type == OPERATOR)
        {
                if (node->data.id_op == SEMICOLON)
                {
                        CompleteOperatorsIR(ir_func, node->left, err_alloc);
                        CompleteOperatorsIR(ir_func, node->right, err_alloc);
                }
                else if (node->data.id_op == OP_ASSIGN)
                {
                        CompleteAssignIR(ir_func, node, err_alloc);
                }
                else if (node->data.id_op == OP_LOOP)
                {
                        CompleteLoopIR(ir_func, node, err_alloc);
                }
                else if (node->data.id_op == OP_CONDITION)
                {
                        CompleteIfIR(ir_func, node, err_alloc);
                }
                else if (node->data.id_op == RET)
                {
                        CompleteReturnIR(ir_func, node, err_alloc);
                }
                else if ((node->data.id_op == INPUT) || (node->data.id_op == OUTPUT))
                {
                        CompleteInOutPutIR(ir_func, node, err_alloc);
                }
                else 
                {
                        INSERT_ERROR_NODE(err_alloc, "invalid tree structure");
                        err_alloc->need_call = true;
                        return;
                }
        }
        else 
        {
                INSERT_ERROR_NODE(err_alloc, "invalid tree structure");
                err_alloc->need_call = true;
                return;
        }
        
        return;
}

void CompleteReturnIR(IR_Function* ir_func, Node* node, err_allocator* err_alloc)
{
        if (node->data.id_op != RET)
        {
                INSERT_ERROR_NODE(err_alloc, "expected ret");
                err_alloc->need_call = true;
                return;
        }

        if (node->left)
                CompleteExpressionIR(ir_func, node->left, err_alloc);
        else if (node->right)
                CompleteExpressionIR(ir_func, node->right, err_alloc);

        char r8[] = "r8";
        char r9[] = "r9";

        char r10[] = "r10";
        char rsp[] = "rsp";

        
        InsertIRblock(ir_func, IR_POP, REG_ARG, 8, r8, NO_ARG, 0, NULL);
        ir_func->position++;

        InsertIRblock(ir_func, IR_MOV, REG_ARG, 4, rsp, REG_ARG, 10, r10);
        ir_func->position++;

        InsertIRblock(ir_func, IR_ADD, REG_ARG, 4, rsp, NUM_ARG, 8, NULL);
        ir_func->position++;



        InsertIRblock(ir_func, IR_POP, REG_ARG, 9, r9, NO_ARG, 0, NULL);
        ir_func->position++;

        InsertIRblock(ir_func, IR_PUSH, REG_ARG, 8, r8, NO_ARG, 0, NULL);
        ir_func->position++;

        InsertIRblock(ir_func, IR_PUSH, REG_ARG, 9, r9, NO_ARG, 0, NULL);
        ir_func->position++;

        InsertIRblock(ir_func, IR_RET, NO_ARG, 0, NULL, NO_ARG, 0, NULL);
        ir_func->position++;

        return;
}

void CompleteInOutPutIR(IR_Function* ir_func, Node* node, err_allocator* err_alloc)
{
        int id_var = 0;
        
        if (node->left)
                id_var = (int) node->left->data.id_var;
        else if (node->right)
                id_var = (int) node->right->data.id_var;

        char r10[] = "r10";
        char rax[] = "rax";
        char mem_r10[] = "[r10]";

        if (node->data.id_op == INPUT)
        {
                char my_input[] = "my_input";
        
                InsertIRblock(ir_func, IR_CALL, LABEL_ARG, 0, my_input, NO_ARG, 0, NULL);
                ir_func->position++;
                
                InsertIRblock(ir_func, IR_PUSH, REG_ARG, 0, rax, NO_ARG, 0, NULL);
                ir_func->position++;
        }       
        else if (node->data.id_op == OUTPUT)
        {
                char my_output[] = "my_output";

                InsertIRblock(ir_func, IR_SUB, REG_ARG, 10, r10, NUM_ARG, 8 * id_var, NULL);
                ir_func->position++;

                InsertIRblock(ir_func, IR_PUSH, MEM_ARG, 0, mem_r10, NO_ARG, 0, NULL);
                ir_func->position++;

                InsertIRblock(ir_func, IR_ADD, REG_ARG, 10, r10, NUM_ARG, 8 * id_var, NULL);
                ir_func->position++;

                InsertIRblock(ir_func, IR_POP, REG_ARG, 0, rax, NO_ARG, 0, NULL);
                ir_func->position++;
                
                InsertIRblock(ir_func, IR_CALL, LABEL_ARG, 0, my_output, NO_ARG, 0, NULL);
                ir_func->position++;
        }
        else
        {
                INSERT_ERROR_NODE(err_alloc, "expected in/out function");
                err_alloc->need_call = true;
                return;
        }

        return;
}


void CompleteAssignIR(IR_Function* ir_func, Node* node, err_allocator* err_alloc)
{
        size_t id_var = 0;
        if (node->left->type == VARIABLE)
        {
                id_var = node->left->data.id_var;
        }
        else
        {
                INSERT_ERROR_NODE(err_alloc, "expected variable");
                err_alloc->need_call = true;
                return;
        }
         
        CompleteExpressionIR(ir_func, node->right, err_alloc); 

        char r10[] = "r10";
        char mem_r10[] = "[r10]";

        InsertIRblock(ir_func, IR_SUB, REG_ARG, 10, r10, NUM_ARG, (int) (8 * id_var), NULL);
        ir_func->position++;

        InsertIRblock(ir_func, IR_POP, MEM_ARG, 0, mem_r10, NO_ARG, 0, NULL);
        ir_func->position++;

        InsertIRblock(ir_func, IR_ADD, REG_ARG, 10, r10, NUM_ARG, (int) (8 * id_var), NULL);
        ir_func->position++;

        if (ir_func->var_num <= id_var)
        {
                char rsp[] = "rsp";

                InsertIRblock(ir_func, IR_SUB, REG_ARG, 4, rsp, NUM_ARG, 8, NULL);
                ir_func->position++;
        }

        if (ir_func->var_num < id_var + 1)
                ir_func->var_num = id_var + 1;

        return;
}


IR_Instruction CompleteBoolExpressionIR(IR_Function* ir_func, Node* node, err_allocator* err_alloc)
{
        CompleteExpressionIR(ir_func, node->left, err_alloc);
        CompleteExpressionIR(ir_func, node->right, err_alloc);

        char r11[] = "r11";
        char r12[] = "r12";

        InsertIRblock(ir_func, IR_POP, REG_ARG, 11, r11, NO_ARG, 0, NULL);
        ir_func->position++;

        InsertIRblock(ir_func, IR_POP, REG_ARG, 12, r12, NO_ARG, 0, NULL);
        ir_func->position++;

        InsertIRblock(ir_func, IR_CMP, REG_ARG, 11, r11, REG_ARG, 12, r12);
        ir_func->position++;

        if (node->data.id_op == OP_ABOVE)
                return IR_JBE;
        else if (node->data.id_op == OP_BELOW)
                return IR_JAE;
        else if (node->data.id_op == OP_EQUAL)
                return IR_JNE;
        else if (node->data.id_op == OP_NO_EQUAL)
                return IR_JE;
        else
        {
                INSERT_ERROR_NODE(err_alloc, "expected conditional jump");
                err_alloc->need_call = true;
                return IR_NOP;
        }
                
        return IR_NOP;
}

void CompleteExpressionIR(IR_Function* ir_func, Node* node, err_allocator* err_alloc)
{
        if (!node) return;

        if (node->type == OPERATOR)
        {
                char r11[] = "r11";
                char r12[] = "r12";
                char rax[] = "rax";

                if (node->left)
                        CompleteExpressionIR(ir_func, node->left, err_alloc);
                if (node->right) 
                        CompleteExpressionIR(ir_func, node->right, err_alloc);
                
                if (node->right)
                {
                        InsertIRblock(ir_func, IR_POP, REG_ARG, 12, r12, NO_ARG, 0, NULL);
                        ir_func->position++;
                }

                if (node->left)
                {
                        InsertIRblock(ir_func, IR_POP, REG_ARG, 11, r11, NO_ARG, 0, NULL);
                        ir_func->position++;
                } 
                
                IR_Instruction instr_id = IR_NOP;

                if (node->data.id_op == OP_ADD)
                        instr_id = IR_ADD;
                else if (node->data.id_op == OP_SUB)
                        instr_id = IR_SUB;
                else if (node->data.id_op == OP_MUL)
                        instr_id = IR_MUL;
                else if (node->data.id_op == OP_DIV)
                        instr_id = IR_DIV;
                else
                {
                        INSERT_ERROR_NODE(err_alloc, "invalid instruction");
                        err_alloc->need_call = true;
                        return;
                }

                if (instr_id == IR_ADD || instr_id == IR_SUB)
                {
                        InsertIRblock(ir_func, instr_id, REG_ARG, 11, r11, REG_ARG, 12, r12);
                        ir_func->position++;

                        InsertIRblock(ir_func, IR_PUSH, REG_ARG, 11, r11, NO_ARG, 0, NULL);
                        ir_func->position++;
                }
                else if (instr_id == IR_MUL || instr_id == IR_DIV)
                {
                        InsertIRblock(ir_func, IR_MOV, REG_ARG, 0, rax, REG_ARG, 11, r11);
                        ir_func->position++;

                        InsertIRblock(ir_func, instr_id, REG_ARG, 12, r12, NO_ARG, 0, NULL);
                        ir_func->position++;

                        InsertIRblock(ir_func, IR_PUSH, REG_ARG, 0, rax, NO_ARG, 0, NULL);
                        ir_func->position++;
                }
        }
        else if (node->type == VARIABLE)
        {
                size_t id_var = node->data.id_var;
                if (ir_func->var_num < id_var + 1)
                        ir_func->var_num = id_var + 1;

                char mem_r10[] = "[r10]";
                char r10[] = "r10";

                InsertIRblock(ir_func, IR_SUB, REG_ARG, 10, r10, NUM_ARG, (int) (8 * id_var), NULL);
                ir_func->position++;

                InsertIRblock(ir_func, IR_PUSH, MEM_ARG, 0, mem_r10, NO_ARG, 0, NULL);
                ir_func->position++;

                InsertIRblock(ir_func, IR_ADD, REG_ARG, 10, r10, NUM_ARG, (int) (8 * id_var), NULL);
                ir_func->position++;
        }
        else if (node->type == FUNCTION)
        {
                char r10[] = "r10";

                InsertIRblock(ir_func, IR_PUSH, REG_ARG, 10, r10, NO_ARG, 0, NULL);
                ir_func->position++;

                InsertIRblock(ir_func, IR_PUSH, NUM_ARG, 0, NULL, NO_ARG, 0, NULL);
                ir_func->position++;

                int arg_num = 0;
                if (node->left)
                        CompletePushArgumentsIR(ir_func, node->left, &arg_num, err_alloc);
                if (node->right)
                        CompletePushArgumentsIR(ir_func, node->right, &arg_num, err_alloc);
                
                char rsp[] = "rsp";

                InsertIRblock(ir_func, IR_ADD, REG_ARG, 4, rsp, NUM_ARG, 8 * (arg_num + 1), NULL);
                ir_func->position++;


                char buf[BUFFER_SIZE] = {};

                int id_fun = (int) node->data.id_fun;
                sprintf(buf, "func_%d", id_fun);

                InsertIRblock(ir_func, IR_CALL, LABEL_ARG, id_fun, buf, NO_ARG, 0, NULL);
                ir_func->position++;

                char r8[] = "r8";
                InsertIRblock(ir_func, IR_POP, REG_ARG, 8, r8, NO_ARG, 0, NULL);
                ir_func->position++;
                
                InsertIRblock(ir_func, IR_POP, REG_ARG, 10, r10, NO_ARG, 0, NULL);
                ir_func->position++;

                InsertIRblock(ir_func, IR_PUSH, REG_ARG, 8, r8, NO_ARG, 0, NULL);
                ir_func->position++;
        }
        else if (node->type == NUMBER)
        {   
                InsertIRblock(ir_func, IR_PUSH, NUM_ARG, (int) node->data.value, NULL, NO_ARG, 0, NULL);
                ir_func->position++;
        }

        
        return;
}


void CompleteLoopIR(IR_Function* ir_func, Node* node, err_allocator* err_alloc)
{
        char start_buf[BUFFER_SIZE] = {};
        char end_buf[BUFFER_SIZE] = {};
        
        sprintf(start_buf, ".while_%lu", ir_func->while_num);
        sprintf(end_buf, ".end_while_%lu", ir_func->while_num);

        InsertIRblock(ir_func, IR_LABEL, LABEL_ARG, 0, start_buf, NO_ARG, 0, NULL);
        memcpy(ir_func->label[ir_func->label_num].label_name, start_buf, strlen(start_buf));
        ir_func->label[ir_func->label_num].offset = ir_func->instrs[ir_func->position].offset;
        ir_func->label_num++;
        ir_func->position++;

        IR_Instruction instr_id = CompleteBoolExpressionIR(ir_func, node->left, err_alloc);

        InsertIRblock(ir_func, instr_id, LABEL_ARG, 0, end_buf, NO_ARG, 0, NULL);
        ir_func->position++;
        
        CompleteOperatorsIR(ir_func, node->right, err_alloc);

        InsertIRblock(ir_func, IR_JMP, LABEL_ARG, 0, start_buf, NO_ARG, 0, NULL);
        ir_func->position++;
        
        InsertIRblock(ir_func, IR_LABEL, LABEL_ARG, 0, end_buf, NO_ARG, 0, NULL);
        memcpy(ir_func->label[ir_func->label_num].label_name, end_buf, strlen(end_buf));
        ir_func->label[ir_func->label_num].offset = ir_func->instrs[ir_func->position].offset;
        ir_func->label_num++;
        ir_func->position++;

        ir_func->while_num++;

        return;   
}

void CompleteIfIR(IR_Function* ir_func, Node* node, err_allocator* err_alloc)
{
        char false_buf[BUFFER_SIZE] = {};
        
        sprintf(false_buf, ".end_if_%lu", ir_func->if_num);

        IR_Instruction instr_id = CompleteBoolExpressionIR(ir_func, node->left, err_alloc);
        
        InsertIRblock(ir_func, instr_id, LABEL_ARG, 0, false_buf, NO_ARG, 0, NULL);
        ir_func->position++;

        CompleteOperatorsIR(ir_func, node->right, err_alloc); 
        
        InsertIRblock(ir_func, IR_LABEL, LABEL_ARG, 0, false_buf, NO_ARG, 0, NULL);
        memcpy(ir_func->label[ir_func->label_num].label_name, false_buf, strlen(false_buf));
        ir_func->label[ir_func->label_num].offset = ir_func->instrs[ir_func->position].offset;
        ir_func->label_num++;
        ir_func->position++;

        ir_func->if_num++;
        return;
}


//--------------------------------------------------------------
// Dump


int DumpInstrName(IR_Instruction ir_instr, char* instr_buf)
{
        switch(ir_instr)
        {
                case IR_NOP:
                        sprintf(instr_buf, "nop       ");    
                        break;
                case IR_MOV:
                        sprintf(instr_buf, "mov       ");    
                        break;    
                case IR_ADD:
                        sprintf(instr_buf, "add       ");    
                        break; 
                case IR_SUB:
                        sprintf(instr_buf, "sub       ");    
                        break;        
                case IR_MUL:
                        sprintf(instr_buf, "mul       ");    
                        break;        
                case IR_DIV:    
                        sprintf(instr_buf, "div       ");
                        break;        
                case IR_INPUT:  
                        sprintf(instr_buf, "INPUT:       ");
                        break;        
                case IR_OUTPUT: 
                        sprintf(instr_buf, "OUTPUT:       ");
                        break;
                case IR_JMP:    
                        sprintf(instr_buf, "jmp       ");
                        break;        
                case IR_JE:     
                        sprintf(instr_buf, "je        ");
                        break;        
                case IR_JNE:    
                        sprintf(instr_buf, "jne       ");
                        break;        
                case IR_JAE:    
                        sprintf(instr_buf, "jae       ");
                        break;        
                case IR_JBE:    
                        sprintf(instr_buf, "jbe       ");
                        break;        
                case IR_CMP:    
                        sprintf(instr_buf, "cmp       ");
                        break;
                case IR_DEFINE:   
                        sprintf(instr_buf, "DEFINE:       ");
                        break;             
                case IR_CALL:   
                        sprintf(instr_buf, "call       ");
                        break;
                case IR_RET:    
                        sprintf(instr_buf, "ret       ");
                        break;        
                case IR_PUSH:   
                        sprintf(instr_buf, "push       ");
                        break;        
                case IR_POP:    
                        sprintf(instr_buf, "pop       ");
                        break;                
                case IR_LABEL:   
                        sprintf(instr_buf, "LABEL:       ");
                        break;
                case IR_SYSCALL:
                        sprintf(instr_buf, "syscall       ");
                        break;
                default:
                        printf("extra IR instr\n");
                        break;
        }
        return 0;
}

int DumpIR(IR_Function* ir_funcs)
{
        IR_block* ir_instrs = NULL;
        IR_Function* func  = NULL;

        char instr_buf[BUFFER_SIZE] = {};

        for (size_t func_index = 0; func_index < MAX_FUNC_NUM && ir_funcs[func_index].size != 0; func_index++)
        {
                func = ir_funcs + func_index;
                printf("\n");

                for (size_t label_index = 0; label_index < func->label_num; label_index++)
                {
                        printf("labels[%lu] = <%s> (offset = %lu)\n", label_index, func->label[label_index].label_name, func->label[label_index].offset);
                }

                for (size_t instr_index = 0; instr_index < func->size; instr_index++)
                {
                        ir_instrs = func->instrs + instr_index;
                        printf("offset = %lu ;", ir_instrs->offset);

                        DumpInstrName(ir_instrs->instr, instr_buf);
                        printf("%s ", instr_buf);
                        memset(instr_buf, 0, BUFFER_SIZE);

                        if (ir_instrs->instr == IR_DEFINE || ir_instrs->instr == IR_CALL)
                        {
                                if (ir_instrs->arg_1.name[0])
                                        printf("arg_1: %s ", ir_instrs->arg_1.name);
                                else
                                        printf("arg_1: func_%d ", ir_instrs->arg_1.value);
                        }
                        else if (ir_instrs->type_1 == VAR_ARG)
                        {
                                printf("arg_1: var_%d ", ir_instrs->arg_1.value);
                        }
                        else if (ir_instrs->type_1 == NUM_ARG)
                        {
                                printf("arg_1: num <%d> ", ir_instrs->arg_1.value);
                        }
                        else if (ir_instrs->type_1 == LABEL_ARG)
                        {
                                printf("arg_1 label <%s> ", ir_instrs->arg_1.name);
                        }
                        else if (ir_instrs->type_1 == STACK_ARG)
                        {
                                printf("arg_1: stack ");
                        }
                        else if (ir_instrs->type_1 == REG_ARG)
                        {
                                printf("arg_1: reg <%s> ", ir_instrs->arg_1.name);
                        }
                        else if (ir_instrs->type_1 == MEM_ARG)
                        {
                                printf("arg_1: memory <%s> ", ir_instrs->arg_1.name);
                        }
                        else if (ir_instrs->type_1 == NO_ARG) 
                        {
                                printf("arg_1: no_arg ");
                        }


                        if (ir_instrs->type_2 == VAR_ARG)
                        {
                                printf("arg_2: var_%d\n", ir_instrs->arg_2.value);
                        }
                        else if (ir_instrs->type_2 == NUM_ARG)
                        {
                                printf("arg_2: num <%d>\n", ir_instrs->arg_2.value);
                        }
                        else if (ir_instrs->type_2 == LABEL_ARG)
                        {
                                printf("arg_2 label <%s>\n", ir_instrs->arg_2.name);
                        }
                        else if (ir_instrs->type_2 == STACK_ARG)
                        {
                                printf("arg_2: stack\n");
                        }
                        else if (ir_instrs->type_2 == REG_ARG)
                        {
                                printf("arg_2: reg <%s>\n", ir_instrs->arg_2.name);
                                
                        }
                        else if (ir_instrs->type_2 == MEM_ARG)
                        {
                                printf("arg_2: memory <%s>\n", ir_instrs->arg_2.name);
                        }
                        else if (ir_instrs->type_2 == NO_ARG) 
                        {
                                printf("arg_2: no_arg\n");
                        }
                        
                        printf("\t opcode: ");
                        for (size_t opcode_index = 0; opcode_index < ir_instrs->x64_instr_size; opcode_index++)
                        {
                                printf("0x%02hhx ", ir_instrs->x64_instr[opcode_index]);
                        }
                        printf("\n");
                        printf("\t bytes: %u\n", ir_instrs->x64_instr_size);

                }
        }

        return 0;
}

int DumpToFile(IR_Function* ir_funcs, const char output_file[], err_allocator* err_alloc)
{
        size_t func_num = 0;
        for (;func_num < MAX_FUNC_NUM && ir_funcs[func_num].size != 0; func_num++)
                ;

        IR_Function* last_func = ir_funcs + func_num - 1;
        Text dump_buf = {};
        size_t dump_buf_size = 20 * (last_func->offset_from_start + last_func->bytes);

        CtorEmptyBuffer(&dump_buf, dump_buf_size, err_alloc);

        IR_block* ir_instrs = NULL;
        IR_Function* func  = NULL;

        char temp_buf[BUFFER_SIZE] = {};
        memset(temp_buf, ' ', BUFFER_SIZE);

        for (size_t func_index = 0; func_index < func_num; func_index++)
        {
                func = ir_funcs + func_index;

                for (size_t instr_index = 0; instr_index < func->size; instr_index++)
                {
                        ir_instrs = func->instrs + instr_index;

                        memset(temp_buf, ' ', BUFFER_SIZE);                                             

                        if (ir_instrs->instr == IR_DEFINE || ir_instrs->instr == IR_LABEL)
                        {
                                sprintf(temp_buf, "\n%s:\n", ir_instrs->arg_1.name);
                                memcpy(dump_buf.str + dump_buf.position, temp_buf, BUFFER_SIZE);
                                dump_buf.position += strlen(temp_buf);

                                memset(temp_buf, ' ', BUFFER_SIZE);
                        }

                        if (ir_instrs->x64_instr_size == 0)
                                continue;
                        
                        ConvertHexToBuf(temp_buf, ir_instrs->x64_instr, ir_instrs->x64_instr_size);
                        memcpy(dump_buf.str + dump_buf.position, temp_buf, BUFFER_SIZE);  
                        dump_buf.position += BUFFER_SIZE;
                        memset(temp_buf, ' ', BUFFER_SIZE);


                        DumpInstrName(ir_instrs->instr, temp_buf);
                        memcpy(dump_buf.str + dump_buf.position, temp_buf, MAX_INSTR_NAME_SIZE);
                        dump_buf.position += MAX_INSTR_NAME_SIZE;
                        memset(temp_buf, ' ', MAX_INSTR_NAME_SIZE);
                        

                        if (ir_instrs->type_1 == NO_ARG)
                        {
                                dump_buf.str[dump_buf.position] = '\n';
                                dump_buf.position++;
                                continue;
                        }
                        else if (ir_instrs->type_1 == NUM_ARG)
                        {
                                sprintf(temp_buf, "%d", ir_instrs->arg_1.value);
                        }
                        else if (ir_instrs->type_1 == LABEL_ARG)
                        {
                                sprintf(temp_buf, "%s", ir_instrs->arg_1.name);
                        }
                        else if (ir_instrs->type_1 == REG_ARG)
                        {
                                sprintf(temp_buf, "%s", ir_instrs->arg_1.name);
                        }
                        else if (ir_instrs->type_1 == MEM_ARG)
                        {
                                sprintf(temp_buf, "%s", ir_instrs->arg_1.name);
                        }

                        size_t arg_size = strlen(temp_buf);
                        memcpy(dump_buf.str + dump_buf.position, temp_buf, arg_size);
                        dump_buf.position += arg_size;
                        memset(temp_buf, ' ', MAX_ARG_NAME_SIZE);


                        arg_size = MAX_ARG_NAME_SIZE - (arg_size % MAX_ARG_NAME_SIZE);
                        memcpy(dump_buf.str + dump_buf.position, temp_buf, arg_size);
                        dump_buf.position += arg_size;
                        memset(temp_buf, ' ', arg_size);

                        if (ir_instrs->type_2 == NO_ARG) 
                        {
                                dump_buf.str[dump_buf.position] = '\n';
                                dump_buf.position++;
                                continue;
                        }

                        dump_buf.str[dump_buf.position] = ',';
                        (dump_buf.position)++;

                        dump_buf.str[dump_buf.position] = ' ';
                        (dump_buf.position)++;
                        
                        if (ir_instrs->type_2 == NUM_ARG)
                        {
                                sprintf(temp_buf, "%d", ir_instrs->arg_2.value);
                        }
                        else if (ir_instrs->type_2 == REG_ARG)
                        {
                                sprintf(temp_buf, "%s", ir_instrs->arg_2.name); 
                        }
                        else if (ir_instrs->type_2 == MEM_ARG)
                        {
                                sprintf(temp_buf, "%s", ir_instrs->arg_2.name);
                        }

                        arg_size = strlen(temp_buf);
                        memcpy(dump_buf.str + dump_buf.position, temp_buf, arg_size);
                        dump_buf.position += arg_size;
                        memset(temp_buf, ' ', MAX_ARG_NAME_SIZE);

                        arg_size = MAX_ARG_NAME_SIZE - (arg_size % MAX_ARG_NAME_SIZE);
                        memcpy(dump_buf.str + dump_buf.position, temp_buf, arg_size);
                        dump_buf.position += arg_size;
                        memset(temp_buf, ' ', arg_size);

                        dump_buf.str[dump_buf.position] = '\n';
                        dump_buf.position++;
                }
        }

        ReallocateBuffer(&dump_buf, dump_buf.position, err_alloc);
        WriteFile(&dump_buf, output_file, err_alloc);

        DtorBuffer(&dump_buf);

        return 0;
}