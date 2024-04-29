#include "AST_to_IR.h"
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

        return 0;
}

int ConvertIR(IR_Function* funcs, Tree** trees, err_allocator* err_alloc)
{
        for (size_t func_index = 0; func_index < NUM_TREE && trees[func_index] != NULL; func_index++)
        {
                CompleteFunctionIR(funcs + func_index, trees[func_index]->root, err_alloc);
                funcs[func_index].size = funcs[func_index].position;
                funcs[func_index].position = 0;
        }
        return 0;
}

int CompleteFunctionIR(IR_Function* funcs, Node* node, err_allocator* err_alloc)
{
        if (node->type == FUNCTION)
        {
                char buf[BUFFER_SIZE] = {};
                int id_fun = (int) node->data.id_fun;
                sprintf(buf, "func_%d", id_fun);
                InsertIRnode(funcs->instrs + funcs->position, IR_DEFINE, LABEL_ARG, id_fun, buf, NO_ARG, 0, NULL);
                funcs->position++;

                if (node->right == NULL)
                {
                        CompleteOperatorsIR(funcs, node->left);
                }
                else
                {       
                        //CompletePopArgumentsIR(funcs, node->left);
                        CompleteOperatorsIR(funcs, node->right);
                }
        }

        return 0;
}

int CompletePopArgumentsIR(IR_Function* funcs, Node* node)
{
        if (!node) {return 0;}

        if (node->type == VARIABLE)
        {
                InsertIRnode(funcs->instrs + funcs->position, IR_POP, VAR_ARG, (int) node->data.id_var, NULL, NO_ARG, 0, NULL);
                funcs->position++;
        }
        else
        {
                CompletePopArgumentsIR(funcs, node->left);
                CompletePopArgumentsIR(funcs, node->right);
        }

        return 0;
}

int CompletePushArgumentsIR(IR_Function* funcs, Node* node, int* arg_num)
{
        if (!node) return 0;

        bool is_comma = false;

        if (node->type == OPERATOR)
                if (node->data.id_op == COMMA)
                        is_comma = true;

        if (is_comma)
        {
                CompletePushArgumentsIR(funcs, node->right, arg_num);
                CompletePushArgumentsIR(funcs, node->left, arg_num);

        }
        else if (!is_comma)
        {
                CompleteExpressionIR(funcs, node);
                (*arg_num)++;
        }

        return 0;
}

void CompleteOperatorsIR(IR_Function* funcs, Node* node)
{
        if (!node) {return;}

        if (node->type == OPERATOR)
        {
                if (node->data.id_op == SEMICOLON)
                {
                        CompleteOperatorsIR(funcs, node->left);
                        CompleteOperatorsIR(funcs, node->right);
                }
                else if (node->data.id_op == OP_ASSIGN)
                {
                        CompleteAssignIR(funcs, node);
                }
                else if (node->data.id_op == OP_LOOP)
                {
                        CompleteLoopIR(funcs, node);
                }
                else if (node->data.id_op == OP_CONDITION)
                {
                        CompleteIfIR(funcs, node);
                }
                else if (node->data.id_op == RET)
                {
                        CompleteReturnIR(funcs, node);
                }
                else if ((node->data.id_op == INPUT) || (node->data.id_op == OUTPUT))
                {
                        CompleteInOutPutIR(funcs, node);
                }
        }
        
        return;
}

void CompleteReturnIR(IR_Function* funcs, Node* node)
{
        if (node->left)
                CompleteExpressionIR(funcs, node->left);
        else if (node->right)
                CompleteExpressionIR(funcs, node->right);
        
        InsertIRnode(funcs->instrs + funcs->position, IR_POP, REG_ARG, 8, "r8", NO_ARG, 0, NULL);
        funcs->position++;

        InsertIRnode(funcs->instrs + funcs->position, IR_POP, REG_ARG, 9, "r9", NO_ARG, 0, NULL);
        funcs->position++;

        InsertIRnode(funcs->instrs + funcs->position, IR_PUSH, REG_ARG, 8, "r8", NO_ARG, 0, NULL);
        funcs->position++;

        InsertIRnode(funcs->instrs + funcs->position, IR_PUSH, REG_ARG, 9, "r9", NO_ARG, 0, NULL);
        funcs->position++;

        InsertIRnode(funcs->instrs + funcs->position, IR_RET, NO_ARG, 0, NULL, NO_ARG, 0, NULL);
        funcs->position++;

        return;
}

void CompleteInOutPutIR(IR_Function* funcs, Node* node)
{
        char buf[BUFFER_SIZE] = {};
        size_t id_var = 0;
        
        if (node->left)
                id_var = node->left->data.id_var;
        else if (node->right)
                id_var = node->right->data.id_var;

        sprintf(buf, "[r10 + %lu]", 8 * id_var);
        
        if (node->data.id_op == INPUT)
        {
                InsertIRnode(funcs->instrs + funcs->position, IR_CALL, LABEL_ARG, 0, "my_input", NO_ARG, 0, NULL);
                funcs->position++;
                
                InsertIRnode(funcs->instrs + funcs->position, IR_MOV, MEM_ARG, id_var, buf, REG_ARG, 1, "rax");
                funcs->position++;
        }       
        else if (node->data.id_op == OUTPUT)
        {
                InsertIRnode(funcs->instrs + funcs->position, IR_MOV,  REG_ARG, 1, "rax", MEM_ARG, id_var, buf);
                funcs->position++;

                InsertIRnode(funcs->instrs + funcs->position, IR_CALL, LABEL_ARG, 0, "my_output", NO_ARG, 0, NULL);
                funcs->position++;
        }
        else
                return; /*error*/

        return;
}


void CompleteAssignIR(IR_Function* funcs, Node* node)
{
        size_t id_var = 0;
        if (node->left->type == VARIABLE)
        {
                id_var = node->left->data.id_var;
        }
        //else /*error*/

        if (funcs->var_num < id_var + 1)
                funcs->var_num = id_var + 1;
         
        CompleteExpressionIR(funcs, node->right); 

        
        InsertIRnode(funcs->instrs + funcs->position, IR_POP, REG_ARG, 8, "r8", NO_ARG, 0, NULL);
        funcs->position++;

        char buf[BUFFER_SIZE] = {};
        sprintf(buf, "[r10 + %d]", (int) (8 * id_var));

        InsertIRnode(funcs->instrs + funcs->position, IR_MOV, MEM_ARG, (int) (8 * id_var), buf, REG_ARG, 8, "r8");
        funcs->position++;

        return;
}


IR_Instruction CompleteBoolExpressionIR(IR_Function* funcs, Node* node)
{
        CompleteExpressionIR(funcs, node->left);
        CompleteExpressionIR(funcs, node->right);

        InsertIRnode(funcs->instrs + funcs->position, IR_POP, REG_ARG, 11, "r11", NO_ARG, 0, NULL);
        funcs->position++;

        InsertIRnode(funcs->instrs + funcs->position, IR_POP, REG_ARG, 12, "r12", NO_ARG, 0, NULL);
        funcs->position++;

        InsertIRnode(funcs->instrs + funcs->position, IR_CMP, REG_ARG, 11, "r11", REG_ARG, 12, "r12");
        funcs->position++;

        if (node->data.id_op == OP_ABOVE)
                return IR_JBE;
        else if (node->data.id_op == OP_BELOW)
                return IR_JAE;
        else if (node->data.id_op == OP_EQUAL)
                return IR_JNE;
        else if (node->data.id_op == OP_NO_EQUAL)
                return IR_JE;
                
        return IR_NOP;
}

void CompleteExpressionIR(IR_Function* funcs, Node* node)
{
        if (!node) return;

        if (node->type == OPERATOR)
        {
                if (node->left)
                        CompleteExpressionIR(funcs, node->left);
                if (node->right) 
                        CompleteExpressionIR(funcs, node->right);
                
                if (node->left)
                {
                        InsertIRnode(funcs->instrs + funcs->position, IR_POP, REG_ARG, 11, "r11", NO_ARG, 0, NULL);
                        funcs->position++;
                }

                if (node->right)
                {
                        InsertIRnode(funcs->instrs + funcs->position, IR_POP, REG_ARG, 12, "r12", NO_ARG, 0, NULL);
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
                        printf("error\n");

                if (instr_id == IR_ADD || instr_id == IR_SUB)
                {
                        InsertIRnode(funcs->instrs + funcs->position, instr_id, REG_ARG, 11, "r11", REG_ARG, 12, "r12");
                        funcs->position++;

                         InsertIRnode(funcs->instrs + funcs->position, IR_PUSH, REG_ARG, 11, "r11", NO_ARG, 0, NULL);
                        funcs->position++;
                }
                else if (instr_id == IR_MUL || instr_id == IR_DIV)
                {
                        InsertIRnode(funcs->instrs + funcs->position, IR_MOV, REG_ARG, 1, "rax", REG_ARG, 11, "r11");
                        funcs->position++;

                        InsertIRnode(funcs->instrs + funcs->position, instr_id, REG_ARG, 12, "r12", NO_ARG, 0, NULL);
                        funcs->position++;

                        InsertIRnode(funcs->instrs + funcs->position, IR_PUSH, REG_ARG, 1, "rax", NO_ARG, 0, NULL);
                        funcs->position++;
                }
        }
        else if (node->type == VARIABLE)
        {
                size_t id_var = node->data.id_var;
                if (funcs->var_num < id_var + 1)
                        funcs->var_num = id_var + 1;

                char buf[BUFFER_SIZE] = {};
                
                sprintf(buf, "[r10 + %lu]", 8 * node->data.id_var);


                InsertIRnode(funcs->instrs + funcs->position, IR_MOV, REG_ARG, 8, "r8", MEM_ARG, 0, buf);
                funcs->position++;

                InsertIRnode(funcs->instrs + funcs->position, IR_PUSH, REG_ARG, 8, "r8", NO_ARG, 0, NULL);
                funcs->position++;
        }
        else if (node->type == FUNCTION)
        {
                int arg_num = 0;
                if (node->left)
                        CompletePushArgumentsIR(funcs, node->left, &arg_num);
                if (node->right)
                        CompletePushArgumentsIR(funcs, node->right, &arg_num);
                
                int num_vars = (int) funcs->var_num;
                
                InsertIRnode(funcs->instrs + funcs->position, IR_ADD, REG_ARG, 10, "r10", NUM_ARG, 8 * num_vars, NULL);        
                funcs->position++;

                char buf[BUFFER_SIZE] = {};

                for (int arg_i = 0; arg_i < arg_num; arg_i++)
                {
                        InsertIRnode(funcs->instrs + funcs->position, IR_POP, REG_ARG, 8, "r8", NO_ARG, 0, NULL);
                        funcs->position++;

                        sprintf(buf, "[r10 + %d]", 8 * arg_i);
                        InsertIRnode(funcs->instrs + funcs->position, IR_MOV, MEM_ARG, 8 * arg_i, buf, REG_ARG, 8, "r8");
                        funcs->position++;
                }

                int id_fun = (int) node->data.id_fun;
                sprintf(buf, "func_%d", id_fun);

                InsertIRnode(funcs->instrs + funcs->position, IR_CALL, LABEL_ARG, id_fun, buf, NO_ARG, 0, NULL);
                funcs->position++;
                
                InsertIRnode(funcs->instrs + funcs->position, IR_SUB, REG_ARG, 10, "r10", NUM_ARG, 8 * num_vars, NULL);        
                funcs->position++;
        }
        else if (node->type == NUMBER)
        {   
                InsertIRnode(funcs->instrs + funcs->position, IR_PUSH, NUM_ARG, (int) node->data.value, NULL, NO_ARG, 0, NULL);

                funcs->position++;
        }
        
        return;
}


void CompleteLoopIR(IR_Function* funcs, Node* node)
{
        char start_buf[BUFFER_SIZE] = {};
        char end_buf[BUFFER_SIZE] = {};
        
        sprintf(start_buf, ".while_%lu", funcs->while_num);
        sprintf(end_buf, ".end_while_%lu", funcs->while_num);

        InsertIRnode(funcs->instrs + funcs->position, IR_LABEL, LABEL_ARG, 0, start_buf, NO_ARG, 0, NULL);
        funcs->position++;

        IR_Instruction instr_id = CompleteBoolExpressionIR(funcs, node->left);

        InsertIRnode(funcs->instrs + funcs->position, instr_id, LABEL_ARG, 0, end_buf, NO_ARG, 0, NULL);
        funcs->position++;
        
        CompleteOperatorsIR(funcs, node->right);

        InsertIRnode(funcs->instrs + funcs->position, IR_JMP, LABEL_ARG, 0, start_buf, NO_ARG, 0, NULL);
        funcs->position++;
        
        InsertIRnode(funcs->instrs + funcs->position, IR_LABEL, LABEL_ARG, 0, end_buf, NO_ARG, 0, NULL);
        funcs->position++;

        funcs->while_num++;

        return;   
}

void CompleteIfIR(IR_Function* funcs, Node* node)
{
        char false_buf[BUFFER_SIZE] = {};
        
        sprintf(false_buf, ".end_if_%lu", funcs->if_num);

        IR_Instruction instr_id = CompleteBoolExpressionIR(funcs, node->left);
        
        InsertIRnode(funcs->instrs + funcs->position, instr_id, LABEL_ARG, 0, false_buf, NO_ARG, 0, NULL);
        funcs->position++;

        CompleteOperatorsIR(funcs, node->right); 
        
        InsertIRnode(funcs->instrs + funcs->position, IR_LABEL, LABEL_ARG, 0, false_buf, NO_ARG, 0, NULL);
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
        }
        return 0;
}

int DumpIR(IR_Function* funcs)
{
        IR_node* ir_instrs = NULL;
        IR_Function* func  = NULL;

        for (size_t i = 0; i < MAX_FUNC_NUM; i++)
        {
                func = funcs + i;
                printf("\n");
                for (size_t j = 0; j < funcs[i].size; j++)
                {
                        ir_instrs = func->instrs + j;
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

                        // printf("\n");
                }
        }

        return 1;
}




//-------------------------------------------------------------------------
// Optimize

int OptimizeIR(IR_Function* func)
{
        IR_node* ir_instr_1 = NULL;
        IR_node* ir_instr_2 = NULL;

        for (size_t instr_index = 0; instr_index < func->size; instr_index++)
        {
                ir_instr_1 = func->instrs + instr_index;
                ir_instr_2 = func->instrs + instr_index + 1;
                
                if (ir_instr_1->instr == IR_PUSH && ir_instr_2->instr == IR_POP)
                {
                        

                        // InsertIRnode(IR_MOV, /*somewhere*/, ir_instr_1->type_1, ir_instr_1->arg_1.value, ir_instr_1->arg_1.name, ir_instr_2->type_1, ir_instr_1->arg_2.value, ir_instr_2->arg_1.name)
                }

                if ((ir_instr_1->instr == IR_MOV) && (ir_instr_1->type_1 == ir_instr_1->type_2) && ((ir_instr_1->arg_2.value == ir_instr_1->arg_1.value)))
                {
                        // DeleteIRnode(/*somewhere*/);
                }
        }

        return 0;
}




int FindMostCommonVars(IR_Function* funcs, int* vars)
{
        // int occurrence_rate[100] = {};

        // for (size_t i = 0; i < index_node; i++)
        // {
        //         if (ir[i].type_1 == VAR_ARG)
        //                 occurrence_rate[ir[i].arg_1.value]++;

        //         if (ir[i].type_2 == VAR_ARG)
        //                 occurrence_rate[ir[i].arg_2.value]++;
        // }

        // for (size_t i = 0; i < 10; i++)
        // {
        //         printf("a[%lu] = %d\n", i, occurrence_rate[i]);
        
        // }

        // qsort(occurrence_rate, 100, sizeof(int), CompareIntDescending);


        return 0;
}

int CompareIntDescending(const void* a, const void* b)
{
        return (*(int*)b - *(int*)a);
}




