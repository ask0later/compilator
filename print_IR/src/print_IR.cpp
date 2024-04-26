#include "print_IR.h"
#include "read_file.h"
#include "tree.h"
#include <cstddef>
#include <cstdlib>
#include <cstring>


const size_t MAX_IR_NODE = 100;


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


        // return 0;
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
        for (size_t i = 0; i < NUM_TREE && trees[i] != NULL; i++)
        {
                CompleteFunctionIR(&funcs[i], trees[i]->root, err_alloc);
                funcs[i].size = funcs[i].position;
                funcs[i].position = 0;
        }


        //int vars[10] = {};
        //FindMostCommonVars(ir, index_node, vars);

        return 0;
}

int CompleteFunctionIR(IR_Function* funcs, Node* node, err_allocator* err_alloc)
{
        if (node->type == FUNCTION)
        {
                InsertIRnode(funcs->instrs + funcs->position, IR_DEFINE, NO_ARG, (int)node->data.id_fun, NULL, NO_ARG, 0, NULL);
                funcs->position++;

                if (node->right == NULL)
                {
                        CompleteOperatorsIR(funcs, node->left);
                }
                else
                {       
                        CompletePopArgumentsIR(funcs, node->left);
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

int CompletePushArgumentsIR(IR_Function* funcs, Node* node)
{
        if (!node) return 0;

        bool is_comma = false;

        if (node->type == OPERATOR)
                if (node->data.id_op == COMMA)
                        is_comma = true;

        if (!is_comma)
        {
                CompleteExpressionIR(funcs, node);
        }
        else if (is_comma)
        {
                CompletePopArgumentsIR(funcs, node->right);
                CompletePopArgumentsIR(funcs, node->left);
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
                        InsertIRnode(funcs->instrs + funcs->position, IR_RET, NO_ARG, 0, NULL, NO_ARG, 0, NULL);
                        funcs->position++;
                }
                else if ((node->data.id_op == INPUT) || (node->data.id_op == OUTPUT))
                {
                        CompleteInOutPutIR(funcs, node);
                }
        }
        
        return;
}

void CompleteInOutPutIR(IR_Function* funcs, Node* node)
{
        IR_Instruction instr_id = IR_NOP;

        if (node->data.id_op == INPUT)
                instr_id  = IR_INPUT;
        else if (node->data.id_op == OUTPUT)
                instr_id  = IR_OUTPUT;
        else
                return; /*error*/
                


        size_t var = 0;

        if (node->left)
                var = node->left->data.id_var;
        else if (node->right)
                var = node->right->data.id_var;
        //else error

        InsertIRnode(funcs->instrs + funcs->position, instr_id, VAR_ARG, (int) var, NULL, NO_ARG, 0, NULL);

        funcs->position++;

        return;
}


void CompleteAssignIR(IR_Function* funcs, Node* node)
{
        int id_var = 0;
        if (node->left->type == VARIABLE)
        {
                id_var = (int) node->left->data.id_var;
        }
        //else /*error*/        
         
        CompleteExpressionIR(funcs, node->right); 


        InsertIRnode(funcs->instrs + funcs->position, IR_MOV, VAR_ARG, id_var, NULL, STACK_ARG, 0, NULL);
        funcs->position++;

        return;
}


IR_Instruction CompleteBoolExpressionIR(IR_Function* funcs, Node* node)
{
        CompleteExpressionIR(funcs, node->left);
        CompleteExpressionIR(funcs, node->right);

        InsertIRnode(funcs->instrs + funcs->position, IR_CMP, STACK_ARG, 0, NULL, STACK_ARG, 0, NULL);
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
                printf("ERORRRRRRRR\n");;
                
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
                
                InsertIRnode(funcs->instrs + funcs->position, IR_POP, REG_ARG, 1, "tmp_1", NO_ARG, 0, NULL);
                funcs->position++;
                InsertIRnode(funcs->instrs + funcs->position, IR_POP, REG_ARG, 2, "tmp_2", NO_ARG, 0, NULL);
                funcs->position++;

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
                        printf("ERORRRRRRRR\n");

                

                InsertIRnode(funcs->instrs + funcs->position, instr_id, REG_ARG, 1, "tmp_1", REG_ARG, 2, "tmp_2");
                funcs->position++;

                InsertIRnode(funcs->instrs + funcs->position, IR_PUSH, REG_ARG, 1, "tmp_1", NO_ARG, 0, NULL);
                funcs->position++;
        }
        else if (node->type == VARIABLE)
        {
                InsertIRnode(funcs->instrs + funcs->position, IR_PUSH, VAR_ARG, (int) node->data.id_var, NULL, NO_ARG, 0, NULL);
                funcs->position++;
        }
        else if (node->type == FUNCTION)
        {
                CompletePushArgumentsIR(funcs, node->left);
                CompletePushArgumentsIR(funcs, node->right);

                InsertIRnode(funcs->instrs + funcs->position, IR_CALL, NO_ARG, (int) node->data.id_fun, NULL, NO_ARG, 0, NULL);

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
        char buf[30] = {};
        char jmp_buf[30] = {};

        char true_buf[30] = {};
        char false_buf[30] = {};
        
        sprintf(true_buf, "while_%lu:", funcs->while_num);
        sprintf(false_buf, "end_while_%lu:", funcs->while_num);
        sprintf(buf, "while_%lu:", funcs->while_num);
        sprintf(jmp_buf, "jmp while_%lu", funcs->while_num);

        InsertIRnode(funcs->instrs + funcs->position, IR_LABEL, NO_ARG, 0, buf, NO_ARG, 0, NULL);
        funcs->position++;

        IR_Instruction instr_id = CompleteBoolExpressionIR(funcs, node->left);

        InsertIRnode(funcs->instrs + funcs->position, instr_id, LABEL_ARG, 0, true_buf, LABEL_ARG, 0, false_buf);
        funcs->position++;


        InsertIRnode(funcs->instrs + funcs->position, IR_LABEL, LABEL_ARG, 0, true_buf, NO_ARG, 0, NULL);
        funcs->position++;
        
        CompleteOperatorsIR(funcs, node->right);

        InsertIRnode(funcs->instrs + funcs->position, IR_JMP, LABEL_ARG, 0, jmp_buf, NO_ARG, 0, NULL);
        funcs->position++;
        

        InsertIRnode(funcs->instrs + funcs->position, IR_LABEL, LABEL_ARG, 0, false_buf, NO_ARG, 0, NULL);
        funcs->position++;

        funcs->while_num++;

        return;   
}

void CompleteIfIR(IR_Function* funcs, Node* node)
{
        char true_buf[30] = {};
        char false_buf[30] = {};
        
        sprintf(true_buf, "if_%lu:", funcs->if_num);
        sprintf(false_buf, "end_if_%lu:", funcs->if_num);

        IR_Instruction instr_id = CompleteBoolExpressionIR(funcs, node->left);
        
        InsertIRnode(funcs->instrs + funcs->position, instr_id, LABEL_ARG, 0, true_buf, LABEL_ARG, 0, false_buf);
        funcs->position++;

        InsertIRnode(funcs->instrs + funcs->position, IR_LABEL, LABEL_ARG, 0, true_buf, NO_ARG, 0, NULL);
        funcs->position++;
        
        CompleteOperatorsIR(funcs, node->right); 
        
        InsertIRnode(funcs->instrs + funcs->position, IR_LABEL, LABEL_ARG, 0, false_buf, NO_ARG, 0, NULL);
        funcs->position++;

        funcs->if_num++;
        return;
}



//-------------------------------------------------------------------------

int CompareIntDescending(const void* a, const void* b)
{
        return (*(int*)b - *(int*)a);
}


int PrintInstrName(IR_Instruction instr)
{
        switch(instr)
        {
                case IR_NOP:
                        //printf("NOP");    
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

                for (size_t j = 0; j < funcs[i].size; j++)
                {
                        ir_instrs = func->instrs + j;
                        PrintInstrName(ir_instrs->instr);

                        if (ir_instrs->instr == IR_DEFINE || ir_instrs->instr == IR_CALL)
                        {
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
                        else if (ir_instrs->type_2 == NO_ARG) 
                        {
                                printf("arg_2: no_arg\n");
                        }

                        // printf("\n");
                }
        }

        return 1;
}
