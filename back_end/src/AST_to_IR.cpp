#include "AST_to_IR.h"
#include "error_allocator.h"
#include "read_file.h"
#include "tree.h"
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>


const size_t MAX_IR_NODE = 100;
const size_t BUFFER_SIZE = 30;


int CtorIR(IR_Function** funcs, err_allocator* err_alloc)
{
        *funcs = (IR_Function*) calloc(MAX_FUNC_NUM, sizeof(IR_Function));

        if (!(*funcs))
        {
                INSERT_ERROR_NODE(err_alloc, "dynamic allocation is fault");
                err_alloc->need_call = true;
                return 1;
        }

        for (size_t i = 0; i < MAX_FUNC_NUM; i++)
        {
                (*funcs)[i].size = 0;
                (*funcs)[i].if_num = 0;
                (*funcs)[i].while_num = 0;
                (*funcs)[i].position = 0;
                
                (*funcs)[i].instrs = (IR_node*) calloc(MAX_IR_NODE, sizeof(IR_node));
                
                if (!((*funcs)[i].instrs))
                {
                        INSERT_ERROR_NODE(err_alloc, "dynamic allocation is fault");
                        err_alloc->need_call = true;
                        return 1;
                }
        }

        return 0;
}


void DtorIR(IR_Function* funcs)
{
        for (size_t i = 0; i < MAX_FUNC_NUM; i++)
        {
                free(funcs[i].instrs);
        }

        free(funcs);
}

int InsertIRnode(IR_node* ir_nodes, IR_Instruction instr_id, IR_type type_1, int arg_1, char* name_1, IR_type type_2, int arg_2, char* name_2)
{
        ir_nodes->instr  = instr_id;

        ir_nodes->type_1 = type_1;
        ir_nodes->arg_1.value = arg_1;

        if (name_1)
                memcpy(ir_nodes->arg_1.name, name_1, strlen(name_1));

        ir_nodes->type_2 = type_2;
        ir_nodes->arg_2.value = arg_2;

        if (name_2)
                memcpy(ir_nodes->arg_2.name, name_2, strlen(name_2));

        InsertOpcode(ir_nodes);

        ir_nodes->offset += (ir_nodes - 1)->x64_instr_size + (ir_nodes - 1)->offset;

        return 0;
}

int InsertOpcode(IR_node* node)
{
        bool match = true;
        for (size_t instr_index = 0; instr_index < sizeof(table) / sizeof(IR_to_opcode); instr_index++)
        {
                if (node->instr == table[instr_index].ir_instr)
                {
                        const args_and_opcode* data_table = (const args_and_opcode*) table[instr_index].data;

                        for (size_t data_index = 0; data_index < sizeof(table[instr_index].data) / sizeof(args_and_opcode); data_index++)
                        {
                                match = true;
                                
                                match &= data_table[data_index].type_1 == node->type_1;                                
                                match &= data_table[data_index].type_2 == node->type_2;
                                
                                if (node->type_1 != NUM_ARG && node->type_1 != LABEL_ARG && node->type_1 != MEM_ARG)
                                        match &= data_table[data_index].arg_1 == node->arg_1.value;
                                
                                if (node->type_2 != NUM_ARG && node->type_2 != MEM_ARG)
                                        match &= data_table[data_index].arg_2  == node->arg_2.value;

                                if (match)
                                {
                                        node->x64_instr_size = data_table[data_index].opcode_size;
                                        memcpy(node->x64_instr, data_table[data_index].opcode, node->x64_instr_size);
                                        break;
                                }
                        }
                        break;
                }
        }


        if (node->type_1 == NUM_ARG || node->type_1 == MEM_ARG)
        {
                int dec_number = node->arg_1.value;
                unsigned char hex_number[sizeof(int)] = {};
                
                ConvertToHex(dec_number, (unsigned char*) hex_number);

                size_t offset = node->x64_instr_size - sizeof(int);
                memcpy(node->x64_instr + offset, hex_number, sizeof(int));
        }

        if (node->type_2 == NUM_ARG)
        {
                int dec_number = node->arg_2.value;
                unsigned char hex_number[sizeof(int)] = {};
                
                ConvertToHex(dec_number, (unsigned char*) hex_number);

                size_t offset = node->x64_instr_size - sizeof(int);
                memcpy(node->x64_instr + offset, hex_number, sizeof(int));
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

int PatchIR(IR_Function* funcs, err_allocator* err_alloc)
{
        Label func_labels[MAX_FUNC_NUM] = {};

        size_t func_num = 0;

        for (; func_num < MAX_FUNC_NUM && funcs[func_num].size != 0; func_num++)
        {
                Label* start_label = funcs[func_num].label;

                strcpy(func_labels[func_num].label_name, start_label->label_name);
                func_labels[func_num].offset = start_label->offset + funcs[func_num].offset_from_start;
        }

        IR_Function* last_func = funcs + func_num - 1;

        Label* in_label  = func_labels + func_num;
        Label* out_label = func_labels + func_num + 1;
        func_num += 2;
        
        memcpy(in_label->label_name, "my_input", sizeof("my_input"));
        in_label->offset = last_func->offset_from_start + last_func->bytes;
     

        memcpy(out_label->label_name, "my_output", sizeof("my_output"));
        out_label->offset = in_label->offset + MY_INPUT_SIZE;

        for (size_t func_index = 0; func_index < func_num; func_index++)
        {
                PatchJumps(funcs + func_index, func_labels, func_num, err_alloc);
        }

        return 0;
}

int PatchJumps(IR_Function* func, Label* func_labels, size_t func_num, err_allocator* err_alloc)
{
        for (size_t instr_index = 0; instr_index < func->size; instr_index++)
        {
                IR_node* node = func->instrs + instr_index;
                
                size_t offset = 0;
                offset += GetOffsetConditionalJumps(node, func, err_alloc);
                offset += GetOffsetGlobalCalls(node, func_labels, func_num, func->offset_from_start, err_alloc);

                if (err_alloc->need_call)
                {
                        INSERT_ERROR_NODE(err_alloc, "unable to determine offset");
                }

                if (offset != 0)
                {
                        unsigned char hex_number[sizeof(int)] = {};
                        ConvertToHex((int) offset, hex_number);
                        size_t offset_index = node->x64_instr_size - sizeof(int);

                        memcpy(node->x64_instr + offset_index, hex_number, sizeof(int));
                }
        }

        return 0;
}

size_t GetOffsetConditionalJumps(IR_node* node, IR_Function* func, err_allocator* err_alloc)
{
        if (node->instr == IR_JE || node->instr == IR_JNE || node->instr == IR_JAE || node->instr == IR_JBE || node->instr == IR_JMP)
        {
                for (size_t label_index = 0; label_index < func->label_num; label_index++)
                {
                        if (strcmp(func->label[label_index].label_name, node->arg_1.name) == 0)
                        {
                                size_t offset = func->label[label_index].offset - (node->offset + node->x64_instr_size);
                                return offset;
                        }
                }
                
                INSERT_ERROR_NODE(err_alloc, "label not declared");
                err_alloc->need_call = true;
        }

        return 0;
}

size_t GetOffsetGlobalCalls(IR_node* node, Label* func_labels, size_t func_num, size_t func_offset, err_allocator* err_alloc)
{
        if (node->instr == IR_CALL)
        {
                for (size_t func_index = 0; func_index < func_num; func_index++)
                {
                        if (strcmp(node->arg_1.name, func_labels[func_index].label_name) == 0)
                        {
                                size_t offset = func_labels[func_index].offset - (node->offset + node->x64_instr_size + func_offset);
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
        InsertIRnode(func->instrs + func->position, IR_PUSH, NUM_ARG, 60, NULL, NO_ARG, 0, NULL);
        func->position++;

        char rax[] = "rax";
        char rdi[] = "rdi";

        InsertIRnode(func->instrs + func->position, IR_POP, REG_ARG, 1, rax, NO_ARG, 0, NULL);
        func->position++;

        InsertIRnode(func->instrs + func->position, IR_PUSH, NUM_ARG, 0, NULL, NO_ARG, 0, NULL);
        func->position++;

        InsertIRnode(func->instrs + func->position, IR_POP, REG_ARG, 7, rdi, NO_ARG, 0, NULL);
        func->position++;

        InsertIRnode(func->instrs + func->position, IR_SYSCALL, NO_ARG, 0, NULL, NO_ARG, 0, NULL);
        func->position++;

        return 0;
}


int ConvertIR(IR_Function* funcs, Tree** trees, err_allocator* err_alloc)
{
        size_t func_num = 0;
        for (;func_num < NUM_TREE && trees[func_num] != NULL; func_num++)
                ;

        func_num--;
                
        IR_Function* main = funcs;

        main->instrs[main->position].instr = IR_NOP;
        main->instrs[main->position].x64_instr[0] = 0x90;
        main->instrs[main->position].x64_instr_size = 1;
        main->position++;

        char r10[] = "r10";
        char RAM_PTR[] = "RAM_PTR";

        InsertIRnode(main->instrs + main->position, IR_MOV, REG_ARG, 10, r10, MEM_ARG, 0, RAM_PTR);
        main->position++;


        CompleteFunctionIR(funcs, trees[func_num]->root, err_alloc);
        if (err_alloc->need_call == true)
        {
                INSERT_ERROR_NODE(err_alloc, "invalid executing CompleteFunctionIR");
                return 1;
        }

        CompleteProgramExit(main);
        
        size_t pos = main->position;
        main->size = pos;

        IR_node* last = main->instrs + pos - 1;

        main->bytes = last->offset + last->x64_instr_size;
        main->position = 0;

        size_t offset_from_start = main->bytes;


        for (size_t func_index = 0; func_index < func_num; func_index++)
        {
                IR_Function* func = funcs + func_index + 1;

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

        PatchIR(funcs, err_alloc);

        return 0;
}

int CompleteFunctionIR(IR_Function* funcs, Node* node, err_allocator* err_alloc)
{
        funcs->instrs[funcs->position].instr = IR_NOP;
        funcs->instrs[funcs->position].x64_instr[0] = 0x90;
        funcs->instrs[funcs->position].x64_instr_size = 1;
        funcs->position++;

        if (node->type == FUNCTION)
        {
                char buf[BUFFER_SIZE] = {};
                int id_fun = (int) node->data.id_fun;
                sprintf(buf, "func_%d", id_fun);

                InsertIRnode(funcs->instrs + funcs->position, IR_DEFINE, LABEL_ARG, id_fun, buf, NO_ARG, 0, NULL);
                
                memcpy(funcs->label[funcs->label_num].label_name, buf, strlen(buf));
                funcs->label[funcs->label_num].offset = funcs->instrs[funcs->position].offset;
                funcs->label_num++;

                funcs->position++;

                if (!node->right)
                        CompleteOperatorsIR(funcs, node->left, err_alloc);
                else
                        CompleteOperatorsIR(funcs, node->right, err_alloc);
        }


        

        return 0;
}

int CompletePushArgumentsIR(IR_Function* funcs, Node* node, int* arg_num, err_allocator* err_alloc)
{
        if (!node) return 0;

        bool is_comma = false;

        if (node->type == OPERATOR)
                if (node->data.id_op == COMMA)
                        is_comma = true;

        if (is_comma)
        {
                CompletePushArgumentsIR(funcs, node->right, arg_num, err_alloc);
                CompletePushArgumentsIR(funcs, node->left, arg_num, err_alloc);

        }
        else if (!is_comma)
        {
                CompleteExpressionIR(funcs, node, err_alloc);
                (*arg_num)++;
        }

        return 0;
}

void CompleteOperatorsIR(IR_Function* funcs, Node* node, err_allocator* err_alloc)
{
        if (!node) {return;}

        if (node->type == OPERATOR)
        {
                if (node->data.id_op == SEMICOLON)
                {
                        CompleteOperatorsIR(funcs, node->left, err_alloc);
                        CompleteOperatorsIR(funcs, node->right, err_alloc);
                }
                else if (node->data.id_op == OP_ASSIGN)
                {
                        CompleteAssignIR(funcs, node, err_alloc);
                }
                else if (node->data.id_op == OP_LOOP)
                {
                        CompleteLoopIR(funcs, node, err_alloc);
                }
                else if (node->data.id_op == OP_CONDITION)
                {
                        CompleteIfIR(funcs, node, err_alloc);
                }
                else if (node->data.id_op == RET)
                {
                        CompleteReturnIR(funcs, node, err_alloc);
                }
                else if ((node->data.id_op == INPUT) || (node->data.id_op == OUTPUT))
                {
                        CompleteInOutPutIR(funcs, node, err_alloc);
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

void CompleteReturnIR(IR_Function* funcs, Node* node, err_allocator* err_alloc)
{
        if (node->data.id_op != RET)
        {
                INSERT_ERROR_NODE(err_alloc, "expected ret");
                err_alloc->need_call = true;
                return;
        }

        if (node->left)
                CompleteExpressionIR(funcs, node->left, err_alloc);
        else if (node->right)
                CompleteExpressionIR(funcs, node->right, err_alloc);

        char r8[] = "r8";
        char r9[] = "r9";
        
        InsertIRnode(funcs->instrs + funcs->position, IR_POP, REG_ARG, 8, r8, NO_ARG, 0, NULL);
        funcs->position++;

        InsertIRnode(funcs->instrs + funcs->position, IR_POP, REG_ARG, 9, r9, NO_ARG, 0, NULL);
        funcs->position++;

        InsertIRnode(funcs->instrs + funcs->position, IR_PUSH, REG_ARG, 8, r8, NO_ARG, 0, NULL);
        funcs->position++;

        InsertIRnode(funcs->instrs + funcs->position, IR_PUSH, REG_ARG, 9, r9, NO_ARG, 0, NULL);
        funcs->position++;

        InsertIRnode(funcs->instrs + funcs->position, IR_RET, NO_ARG, 0, NULL, NO_ARG, 0, NULL);
        funcs->position++;

        return;
}

void CompleteInOutPutIR(IR_Function* funcs, Node* node, err_allocator* err_alloc)
{
        char buf[BUFFER_SIZE] = {};
        int id_var = 0;
        
        if (node->left)
                id_var = (int) node->left->data.id_var;
        else if (node->right)
                id_var = (int) node->right->data.id_var;

        sprintf(buf, "[r10 + %d]", 8 * id_var);
        
        char rax[] = "rax";

        if (node->data.id_op == INPUT)
        {
                char my_input[] = "my_input";
        
                InsertIRnode(funcs->instrs + funcs->position, IR_CALL, LABEL_ARG, 0, my_input, NO_ARG, 0, NULL);
                funcs->position++;
                
                InsertIRnode(funcs->instrs + funcs->position, IR_PUSH, REG_ARG, 1, rax, NO_ARG, 0, NULL);
                funcs->position++;

                InsertIRnode(funcs->instrs + funcs->position, IR_POP, MEM_ARG, 8 * id_var, buf, NO_ARG, 0, NULL);
                funcs->position++;
        }       
        else if (node->data.id_op == OUTPUT)
        {
                char my_output[] = "my_output";

                InsertIRnode(funcs->instrs + funcs->position, IR_PUSH, MEM_ARG, 8 * id_var, buf, NO_ARG, 0, NULL);
                funcs->position++;

                InsertIRnode(funcs->instrs + funcs->position, IR_POP, REG_ARG, 1, rax, NO_ARG, 0, NULL);
                funcs->position++;
                
                InsertIRnode(funcs->instrs + funcs->position, IR_CALL, LABEL_ARG, 0, my_output, NO_ARG, 0, NULL);
                funcs->position++;
        }
        else
        {
                INSERT_ERROR_NODE(err_alloc, "expected in/out function");
                err_alloc->need_call = true;
                return;
        }

        return;
}


void CompleteAssignIR(IR_Function* funcs, Node* node, err_allocator* err_alloc)
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

        if (funcs->var_num < id_var + 1)
                funcs->var_num = id_var + 1;
         
        CompleteExpressionIR(funcs, node->right, err_alloc); 

        char buf[BUFFER_SIZE] = {};
        sprintf(buf, "[r10 + %d]", (int) (8 * id_var));

        InsertIRnode(funcs->instrs + funcs->position, IR_POP, MEM_ARG, (int) (8 * id_var), buf, NO_ARG, 0, NULL);
        funcs->position++;

        return;
}


IR_Instruction CompleteBoolExpressionIR(IR_Function* funcs, Node* node, err_allocator* err_alloc)
{
        CompleteExpressionIR(funcs, node->left, err_alloc);
        CompleteExpressionIR(funcs, node->right, err_alloc);

        char r11[] = "r11";
        char r12[] = "r12";

        InsertIRnode(funcs->instrs + funcs->position, IR_POP, REG_ARG, 11, r11, NO_ARG, 0, NULL);
        funcs->position++;

        InsertIRnode(funcs->instrs + funcs->position, IR_POP, REG_ARG, 12, r12, NO_ARG, 0, NULL);
        funcs->position++;

        InsertIRnode(funcs->instrs + funcs->position, IR_CMP, REG_ARG, 11, r11, REG_ARG, 12, r12);
        funcs->position++;

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

void CompleteExpressionIR(IR_Function* funcs, Node* node, err_allocator* err_alloc)
{
        if (!node) return;

        if (node->type == OPERATOR)
        {
                char r11[] = "r11";
                char r12[] = "r12";
                char rax[] = "rax";

                if (node->left)
                        CompleteExpressionIR(funcs, node->left, err_alloc);
                if (node->right) 
                        CompleteExpressionIR(funcs, node->right, err_alloc);
                
                if (node->right)
                {
                        InsertIRnode(funcs->instrs + funcs->position, IR_POP, REG_ARG, 12, r12, NO_ARG, 0, NULL);
                        funcs->position++;
                }

                if (node->left)
                {
                        InsertIRnode(funcs->instrs + funcs->position, IR_POP, REG_ARG, 11, r11, NO_ARG, 0, NULL);
                        funcs->position++;
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
                        InsertIRnode(funcs->instrs + funcs->position, instr_id, REG_ARG, 11, r11, REG_ARG, 12, r12);
                        funcs->position++;

                        InsertIRnode(funcs->instrs + funcs->position, IR_PUSH, REG_ARG, 11, r11, NO_ARG, 0, NULL);
                        funcs->position++;
                }
                else if (instr_id == IR_MUL || instr_id == IR_DIV)
                {
                        InsertIRnode(funcs->instrs + funcs->position, IR_MOV, REG_ARG, 1, rax, REG_ARG, 11, r11);
                        funcs->position++;

                        InsertIRnode(funcs->instrs + funcs->position, instr_id, REG_ARG, 12, r12, NO_ARG, 0, NULL);
                        funcs->position++;

                        InsertIRnode(funcs->instrs + funcs->position, IR_PUSH, REG_ARG, 1, rax, NO_ARG, 0, NULL);
                        funcs->position++;
                }
        }
        else if (node->type == VARIABLE)
        {
                size_t id_var = node->data.id_var;
                if (funcs->var_num < id_var + 1)
                        funcs->var_num = id_var + 1;

                char buf[BUFFER_SIZE] = {};
                sprintf(buf, "[r10 + %lu]", 8 * id_var);

                InsertIRnode(funcs->instrs + funcs->position, IR_PUSH, MEM_ARG, (int) (8 * id_var), buf, NO_ARG, 0, NULL);
                funcs->position++;
        }
        else if (node->type == FUNCTION)
        {
                int arg_num = 0;
                if (node->left)
                        CompletePushArgumentsIR(funcs, node->left, &arg_num, err_alloc);
                if (node->right)
                        CompletePushArgumentsIR(funcs, node->right, &arg_num, err_alloc);
                
                int num_vars = (int) funcs->var_num;

                char r10[] = "r10";
                
                InsertIRnode(funcs->instrs + funcs->position, IR_ADD, REG_ARG, 10, r10, NUM_ARG, 8 * num_vars, NULL);        
                funcs->position++;

                char buf[BUFFER_SIZE] = {};

                for (int arg_i = 0; arg_i < arg_num; arg_i++)
                {
                        sprintf(buf, "[r10 + %d]", 8 * arg_i);

                        InsertIRnode(funcs->instrs + funcs->position, IR_POP, MEM_ARG, 8 * arg_i, buf, NO_ARG, 0, NULL);
                        funcs->position++;
                }

                int id_fun = (int) node->data.id_fun;
                sprintf(buf, "func_%d", id_fun);

                InsertIRnode(funcs->instrs + funcs->position, IR_CALL, LABEL_ARG, id_fun, buf, NO_ARG, 0, NULL);
                funcs->position++;
                
                InsertIRnode(funcs->instrs + funcs->position, IR_SUB, REG_ARG, 10, r10, NUM_ARG, 8 * num_vars, NULL);
                funcs->position++;
        }
        else if (node->type == NUMBER)
        {   
                InsertIRnode(funcs->instrs + funcs->position, IR_PUSH, NUM_ARG, (int) node->data.value, NULL, NO_ARG, 0, NULL);
                funcs->position++;
        }

        
        return;
}


void CompleteLoopIR(IR_Function* funcs, Node* node, err_allocator* err_alloc)
{
        char start_buf[BUFFER_SIZE] = {};
        char end_buf[BUFFER_SIZE] = {};
        
        sprintf(start_buf, ".while_%lu", funcs->while_num);
        sprintf(end_buf, ".end_while_%lu", funcs->while_num);

        InsertIRnode(funcs->instrs + funcs->position, IR_LABEL, LABEL_ARG, 0, start_buf, NO_ARG, 0, NULL);
        memcpy(funcs->label[funcs->label_num].label_name, start_buf, strlen(start_buf));
        funcs->label[funcs->label_num].offset = funcs->instrs[funcs->position].offset;
        funcs->label_num++;
        funcs->position++;

        IR_Instruction instr_id = CompleteBoolExpressionIR(funcs, node->left, err_alloc);

        InsertIRnode(funcs->instrs + funcs->position, instr_id, LABEL_ARG, 0, end_buf, NO_ARG, 0, NULL);
        funcs->position++;
        
        CompleteOperatorsIR(funcs, node->right, err_alloc);

        InsertIRnode(funcs->instrs + funcs->position, IR_JMP, LABEL_ARG, 0, start_buf, NO_ARG, 0, NULL);
        funcs->position++;
        
        InsertIRnode(funcs->instrs + funcs->position, IR_LABEL, LABEL_ARG, 0, end_buf, NO_ARG, 0, NULL);
        memcpy(funcs->label[funcs->label_num].label_name, end_buf, strlen(end_buf));
        funcs->label[funcs->label_num].offset = funcs->instrs[funcs->position].offset;
        funcs->label_num++;
        funcs->position++;

        funcs->while_num++;

        return;   
}

void CompleteIfIR(IR_Function* funcs, Node* node, err_allocator* err_alloc)
{
        char false_buf[BUFFER_SIZE] = {};
        
        sprintf(false_buf, ".end_if_%lu", funcs->if_num);

        IR_Instruction instr_id = CompleteBoolExpressionIR(funcs, node->left, err_alloc);
        
        InsertIRnode(funcs->instrs + funcs->position, instr_id, LABEL_ARG, 0, false_buf, NO_ARG, 0, NULL);
        funcs->position++;

        CompleteOperatorsIR(funcs, node->right, err_alloc); 
        
        InsertIRnode(funcs->instrs + funcs->position, IR_LABEL, LABEL_ARG, 0, false_buf, NO_ARG, 0, NULL);
        memcpy(funcs->label[funcs->label_num].label_name, false_buf, strlen(false_buf));
        funcs->label[funcs->label_num].offset = funcs->instrs[funcs->position].offset;
        funcs->label_num++;
        funcs->position++;

        funcs->if_num++;
        return;
}


//--------------------------------------------------------------
// Dump


int DumpInstrName(IR_Instruction instr)
{
        switch(instr)
        {
                case IR_NOP:
                        printf("NOP: ");    
                        break;
                case IR_MOV:
                        printf("MOV: ");    
                        break;    
                case IR_ADD:
                        printf("ADD: ");    
                        break; 
                case IR_SUB:
                        printf("SUB: ");    
                        break;        
                case IR_MUL:
                        printf("MUL: ");    
                        break;        
                case IR_DIV:    
                        printf("DIV: ");
                        break;        
                case IR_INPUT:  
                        printf("INPUT: ");
                        break;        
                case IR_OUTPUT: 
                        printf("OUTPUT: ");
                        break;
                case IR_JMP:    
                        printf("JMP: ");
                        break;        
                case IR_JE:     
                        printf("JE: ");
                        break;        
                case IR_JNE:    
                        printf("JNE: ");
                        break;        
                case IR_JAE:    
                        printf("JAE: ");
                        break;        
                case IR_JBE:    
                        printf("JBE: ");
                        break;        
                case IR_CMP:    
                        printf("CMP: ");
                        break;
                case IR_DEFINE:   
                        printf("DEFINE: ");
                        break;             
                case IR_CALL:   
                        printf("CALL: ");
                        break;
                case IR_RET:    
                        printf("RET: ");
                        break;        
                case IR_PUSH:   
                        printf("PUSH: ");
                        break;        
                case IR_POP:    
                        printf("POP: ");
                        break;                
                case IR_LABEL:   
                        printf("LABEL: ");
                        break;
                case IR_SYSCALL:
                        printf("SYSCALL: ");
                        break;
                default:
                        printf("extra IR instr\n");
                        break;
        }
        return 0;
}

int DumpIR(IR_Function* funcs)
{
        IR_node* ir_instrs = NULL;
        IR_Function* func  = NULL;

        for (size_t func_index = 0; func_index < MAX_FUNC_NUM; func_index++)
        {
                func = funcs + func_index;
                printf("\n");

                for (size_t label_index = 0; label_index < func->label_num; label_index++)
                {
                        printf("labels[%lu] = <%s> (offset = %lu)\n", label_index, func->label[label_index].label_name, func->label[label_index].offset);
                }

                for (size_t instr_index = 0; instr_index < func->size; instr_index++)
                {
                        ir_instrs = func->instrs + instr_index;
                        printf("offset = %lu ;", ir_instrs->offset);
                        DumpInstrName(ir_instrs->instr);

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




//-------------------------------------------------------------------------
// Optimize

// int OptimizeIR(IR_Function* func)
// {
//         IR_node* ir_instr_1 = NULL;
//         IR_node* ir_instr_2 = NULL;

//         for (size_t instr_index = 0; instr_index < func->size; instr_index++)
//         {
//                 ir_instr_1 = func->instrs + instr_index;
//                 ir_instr_2 = func->instrs + instr_index + 1;
                
//                 if (ir_instr_1->instr == IR_PUSH && ir_instr_2->instr == IR_POP)
//                 {
                        

//                         // InsertIRnode(IR_MOV, /*somewhere*/, ir_instr_1->type_1, ir_instr_1->arg_1.value, ir_instr_1->arg_1.name, ir_instr_2->type_1, ir_instr_1->arg_2.value, ir_instr_2->arg_1.name)
//                 }

//                 if ((ir_instr_1->instr == IR_MOV) && (ir_instr_1->type_1 == ir_instr_1->type_2) && ((ir_instr_1->arg_2.value == ir_instr_1->arg_1.value)))
//                 {
//                         // DeleteIRnode(/*somewhere*/);
//                 }
//         }

//         return 0;
// }




// int FindMostCommonVars(IR_Function* funcs, int* vars)
// {
//         // int occurrence_rate[100] = {};

//         // for (size_t i = 0; i < index_node; i++)
//         // {
//         //         if (ir[i].type_1 == VAR_ARG)
//         //                 occurrence_rate[ir[i].arg_1.value]++;

//         //         if (ir[i].type_2 == VAR_ARG)
//         //                 occurrence_rate[ir[i].arg_2.value]++;
//         // }

//         // for (size_t i = 0; i < 10; i++)
//         // {
//         //         printf("a[%lu] = %d\n", i, occurrence_rate[i]);
        
//         // }

//         // qsort(occurrence_rate, 100, sizeof(int), CompareIntDescending);


//         return 0;
// }

// int CompareIntDescending(const void* a, const void* b)
// {
//         return (*(int*)b - *(int*)a);
// }




