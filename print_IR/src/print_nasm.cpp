#include "print_nasm.h"
#include "read_file.h"
#include "tree.h"
#include <cstddef>
#include <cstring>


const size_t STACK_SIZE  = 25;
const size_t RAM_SIZE    = 50;

const size_t MAX_INSTR_NUM = 1000;



int PrintInstructions(Instruction* instrs)
{
    for (size_t i = 0; i < 100; i++)
    {
        printf("%s\n", instrs[i].asm_instr);
    }

    return 0;
}

int CtorInstructions(Instruction** instrs, err_allocator* err_alloc)
{
        (*instrs) = (Instruction*) calloc(MAX_INSTR_NUM, sizeof(Instruction));
        if (!(*instrs))
        {
                INSERT_ERROR_NODE(err_alloc, "dynamic allocation is fault");
                err_alloc->need_call = true;
                return 1;
        }

        return 0;
}


void DtorInstructions(Instruction** instrs)
{
        free(*instrs);
        return;
}


int InsertInstruction(Instruction* instrs, char* asm_insr, size_t asm_instr_size, unsigned long* x86_instr, short instr_size, size_t position)
{
    memcpy(instrs->asm_instr, asm_insr, asm_instr_size);

    memcpy(instrs->x86_64_instr, (char*) x86_instr, (size_t) instr_size);
    
    instrs->instr_size = instr_size;

    return 0;
}

int CompleteInstructions(Instruction** instrs, Tree** trees, err_allocator* err_alloc)
{
    size_t index_node = 0;

    char buf[50] = {};


    memcpy((*instrs)[index_node].asm_instr, "mov r12, STACK_PTR", sizeof("mov r12, STACK_PTR"));
    index_node++;

    for (size_t i = 0; i < NUM_TREE && trees[i] != NULL; i++)
    {
         
        CompleteFunction(*instrs, &index_node, trees[i]->root, err_alloc);
    }

    sprintf(buf, "\nRAM_PTR: dq %lu dup(0)", RAM_SIZE);
    memcpy((*instrs)[index_node].asm_instr, buf, strlen(buf));
    index_node++;
    
    memset(buf, 0, 50);

    sprintf(buf, "STACK_PTR: dq %lu dup(0)", STACK_SIZE);
    memcpy((*instrs)[index_node].asm_instr, buf, strlen(buf));
    index_node++;

    return 0;
}

int CompleteFunction(Instruction* instrs, size_t* index_node, Node* node, err_allocator* err_alloc)
{
    if (node->type == FUNCTION)
    {
        char buf[10] = {};

        // r12 - stack pointer

        sprintf(buf, "\nfun_%lu:", node->data.id_fun);
        memcpy(instrs[*index_node].asm_instr, buf, strlen(buf));

        (*index_node)++;

        
        size_t num_if = 0, num_while = 0;

        if (node->right == NULL)
        {
            CompleteOperators(instrs, index_node, node->left, &num_while, &num_if);
        }
        else
        {
            CompleteArgFuncDef(instrs, index_node, node->left);
            CompleteOperators(instrs, index_node, node->right, &num_while, &num_if);
        }
    }

    return 0;
}

void CompleteArgFuncDef(Instruction* instrs, size_t* index_node, Node* node)
{
    if (!node) {return;}

    if (node->type == VARIABLE)
    {
        CompleteVariable(instrs, index_node, node);

        memcpy(instrs[*index_node].asm_instr, "sub r12, 8", sizeof("sub r12, 8"));
        // 49 83 EC 08
        (*index_node)++;

        memcpy(instrs[*index_node].asm_instr, "mov r10, [r12]", sizeof("mov r10, [r12]"));
        // 
        (*index_node)++;

        memcpy(instrs[*index_node].asm_instr, "mov [r11], r10", sizeof("mov [r11], r10"));
        (*index_node)++;
    }
    else
    {
        CompleteArgFuncDef(instrs, index_node, node->left);
        CompleteArgFuncDef(instrs, index_node, node->right);
    }

    return;
}



void CompleteOperators(Instruction* instrs, size_t* index_node, Node* node, size_t* num_while, size_t* num_if)
{
    if (!node) {return;}

    if (node->type == OPERATOR)
    {
        if (node->data.id_op == SEMICOLON)
        {
           CompleteOperators(instrs, index_node, node->left, num_while, num_if);
           CompleteOperators(instrs, index_node, node->right, num_while, num_if);
        }
        else if (node->data.id_op == OP_ASSIGN)
        {
           CompleteAssign(instrs, index_node, node);
        }
        else if (node->data.id_op == OP_LOOP)
        {
           CompleteLoop(instrs, index_node, node, num_while, num_if);
        }
        else if (node->data.id_op == OP_CONDITION)
        {
           CompleteIf(instrs, index_node, node, num_while, num_if);
        }
        else if (node->data.id_op == RET)
        {
           CompleteReturn(instrs, index_node, node);
        }
        else if ((node->data.id_op == INPUT) || (node->data.id_op == OUTPUT))
        {
           CompleteInOutPut(instrs, index_node, node);
        }
    }
    
    return;
}


void CompleteInOutPut(Instruction* instrs, size_t* index_node, Node* node)
{
    if (node->data.id_op == INPUT)
    {
        memcpy(instrs[*index_node].asm_instr, "call my_input", sizeof("call my_input"));
        // E8 ** 00 00
        (*index_node)++;

        CompleteVariable(instrs, index_node, node->left);
        CompleteVariable(instrs, index_node, node->right);

        memcpy(instrs[*index_node].asm_instr, "sub r12, 8", sizeof("sub r12, 8"));
        // 49 83 EC 08
        (*index_node)++;

        memcpy(instrs[*index_node].asm_instr, "mov r10, [r12]", sizeof("mov r10, [r12]"));
        (*index_node)++;

        memcpy(instrs[*index_node].asm_instr, "mov [r11], r10", sizeof("mov [r11], r10"));
        (*index_node)++;  
    }
    else if (node->data.id_op == OUTPUT)
    {
        CompleteVariable(instrs, index_node, node->left);
        CompleteVariable(instrs, index_node, node->right);

        memcpy(instrs[*index_node].asm_instr, "mov r10, [r11]", sizeof("mov r10, [r11]"));
        (*index_node)++;

        memcpy(instrs[*index_node].asm_instr, "mov [r12], r10", sizeof("mov [r12], r10"));
        (*index_node)++;

        memcpy(instrs[*index_node].asm_instr, "add r12, 8", sizeof("add r12, 8"));
        // 49 83 C4 08
        (*index_node)++;

        memcpy(instrs[*index_node].asm_instr, "call my_output", sizeof("call my_output"));
        // E8 ** 00 00
        (*index_node)++;       
    }
}

void CompleteLoop(Instruction* instrs, size_t* index_node, Node* node, size_t* num_while, size_t* num_if)
{
    size_t old_num_while = *num_while;
    (*num_while)++;
    
    char buf[30] = {};
    sprintf(buf, "while_%lu:", old_num_while);
    memcpy(instrs[*index_node].asm_instr, buf, strlen(buf));
    (*index_node)++;

    memset(buf, 0, 30);

    CompleteBoolExpression(instrs, index_node, node->left);
    
    sprintf(buf, "end_while_%lu", old_num_while);
    strcat(instrs[*index_node].asm_instr, buf);

    (*index_node)++;

    memset(buf, 0, 30);

    CompleteOperators(instrs, index_node, node->right, num_while, num_if);

    sprintf(buf, "jmp while_%lu", old_num_while);
    // 
    memcpy(instrs[*index_node].asm_instr, buf, strlen(buf));
    (*index_node)++;

    memset(buf, 0, 30);

    sprintf(buf, "end_while_%lu:", old_num_while);
    memcpy(instrs[*index_node].asm_instr, buf, strlen(buf));
    (*index_node)++;

    return;
}

void CompleteIf(Instruction* instrs, size_t* index_node, Node* node, size_t* num_while, size_t* num_if)
{
    size_t old_num_if = *num_if;
    (*num_if)++;

    CompleteBoolExpression(instrs, index_node, node->left);

    char buf[30] = {};

    sprintf(buf, "end_if_%lu", old_num_if);
    strcat(instrs[*index_node].asm_instr, buf);
    (*index_node)++;

    memset(buf, 0, 30);

    CompleteOperators(instrs, index_node, node->right, num_while, num_if);

    sprintf(buf, "end_if_%lu:", old_num_if);
    memcpy(instrs[*index_node].asm_instr, buf, strlen(buf));
    (*index_node)++;
}

void CompleteReturn(Instruction* instrs, size_t* index_node, Node* node)
{
    if (node->right)
        CompleteExpression(instrs, index_node, node->right);
    else if (node->left)
        CompleteExpression(instrs, index_node, node->left);

    memcpy(instrs[*index_node].asm_instr, "ret\n", sizeof("ret\n"));
    // C3
    (*index_node)++; 

    return;
}

void CompleteAssign(Instruction* instrs, size_t* index_node, Node* node)
{
    CompleteExpression(instrs, index_node, node->right);
    CompleteVariable(instrs, index_node, node->left);

    memcpy(instrs[*index_node].asm_instr, "sub r12, 8", sizeof("sub r12, 8"));
    // 49 83 EC 08
    (*index_node)++;

    memcpy(instrs[*index_node].asm_instr, "mov r10, [r12]", sizeof("mov r10, [r12]"));
    (*index_node)++;

    memcpy(instrs[*index_node].asm_instr, "mov [r11], r10", sizeof("mov [r11], r10"));
    (*index_node)++;    

    return;
}



void CompleteArgFuncAnnoun(Instruction* instrs, size_t* index_node, Node* node)
{
    if (!node) return;

    bool is_comma = false;

    if (node->type == OPERATOR)
        if (node->data.id_op == COMMA)
            is_comma = true;
    
    if (!is_comma)
    {
       CompleteExpression(instrs, index_node, node);
    }
    else if (is_comma)
    {
        CompleteArgFuncAnnoun(instrs, index_node, node->right);
        CompleteArgFuncAnnoun(instrs, index_node, node->left);
    }
}

void CompleteBoolExpression(Instruction* instrs, size_t* index_node, Node* node)
{
    CompleteExpression(instrs, index_node, node->left);
    CompleteExpression(instrs, index_node, node->right);

    if (node->data.id_op == OP_ABOVE)
    {
        memcpy(instrs[*index_node].asm_instr, "jbe ", sizeof("jbe "));
        // 76
    }
    else if (node->data.id_op == OP_BELOW)
    {
        memcpy(instrs[*index_node].asm_instr, "jae ", sizeof("jae "));
        // 73
    }
    else if (node->data.id_op == OP_EQUAL)
    {
        memcpy(instrs[*index_node].asm_instr, "jne ", sizeof("jne "));
        // 75
    }
    else if (node->data.id_op == OP_NO_EQUAL)
    {
        memcpy(instrs[*index_node].asm_instr, "je ", sizeof("je "));
        // 74
    }
    return;
}

void CompleteExpression(Instruction* instrs, size_t* index_node, Node* node)
{
    if (!node) return;

    if (node->type == OPERATOR)
    {
        if (node->left)
        {   
            CompleteExpression(instrs, index_node, node->left);
        }
        if (node->right) 
        {
            CompleteExpression(instrs, index_node, node->right);
        }

        memcpy(instrs[*index_node].asm_instr, "sub r12, 8", sizeof("sub r12, 8"));
        // 49 83 EC 08
        (*index_node)++;

        fprintf(stderr, "%lu\n", *index_node);
        memcpy(instrs[*index_node].asm_instr, "mov rax, [r12]", sizeof("mov rdi, [r12]"));
        (*index_node)++;

        memcpy(instrs[*index_node].asm_instr, "sub r12, 8", sizeof("sub r12, 8"));
        // 49 83 EC 08
        (*index_node)++;

        memcpy(instrs[*index_node].asm_instr, "mov rdx, [r12]", sizeof("mov rsi, [r12]"));
        (*index_node)++;

        char buf[30] = {};


        if (node->data.id_op == OP_ADD)
        {
            sprintf(buf, "add rax, rdx");

            memcpy(instrs[*index_node].asm_instr, "mov [r12], rax", sizeof("mov [r12], rax"));
            (*index_node)++;

            memcpy(instrs[*index_node].asm_instr, "add r12, 8", sizeof("add r12, 8"));
            (*index_node)++;
        }
        else if (node->data.id_op == OP_SUB)
        {
            sprintf(buf, "sub rax, rdx");

            memcpy(instrs[*index_node].asm_instr, "mov [r12], rax", sizeof("mov [r12], rax"));
            (*index_node)++;

            memcpy(instrs[*index_node].asm_instr, "add r12, 8", sizeof("add r12, 8"));
            (*index_node)++;
        }
        else if (node->data.id_op == OP_MUL)
        {
            sprintf(buf, "mul rax, rdx");

            memcpy(instrs[*index_node].asm_instr, "mov [r12], rax", sizeof("mov [r12], rax"));
            (*index_node)++;

            memcpy(instrs[*index_node].asm_instr, "add r12, 8", sizeof("add r12, 8"));
            // 49 83 C4 01
            (*index_node)++;
        }
        else if (node->data.id_op == OP_DIV)
        {
            sprintf(buf, "div rax, rdx");

            memcpy(instrs[*index_node].asm_instr, "mov [r12], rax", sizeof("mov [r12], rax"));
            (*index_node)++;

            memcpy(instrs[*index_node].asm_instr, "add r12, 8", sizeof("add r12, 8"));
            // 49 83 C4 01
            (*index_node)++;
        }

        memcpy(instrs[*index_node].asm_instr, buf, strlen(buf));
        (*index_node)++;

        // else if (node->data.id_op == FUN_SQRT)
        // {

        // }
        //     fprintf(To, "sqrt \n");
        // else if (node->data.id_op == FUN_SIN)
        //     fprintf(To, "sin \n");
        // else if (node->data.id_op == FUN_COS)
        //     fprintf(To, "cos \n");
        
    }
    else if (node->type == VARIABLE)
    {
        CompleteVariable(instrs, index_node, node);
        
        memcpy(instrs[*index_node].asm_instr, "mov r10, [r11]", sizeof("mov r10, [r11]"));
        (*index_node)++;

        memcpy(instrs[*index_node].asm_instr, "mov [r12], r10", sizeof("mov [r12], r10"));
        (*index_node)++;

        memcpy(instrs[*index_node].asm_instr, "add r12, 8", sizeof("add r12, 8"));
        // 49 83 C4 08
        (*index_node)++;
    }
    else if (node->type == FUNCTION)
    {
        CompleteArgFuncAnnoun(instrs, index_node, node->left);
        CompleteArgFuncAnnoun(instrs, index_node, node->right);

        char buf[30] = {};

        sprintf(buf, "call fun_%lu", node->data.id_fun);

        memcpy(instrs[*index_node].asm_instr, buf, strlen(buf));
        (*index_node)++;
    }
    else if (node->type == NUMBER)
    {   
        char buf[30] = {};
        
        sprintf(buf, "mov [r12], %d", (int) node->data.value);

        memcpy(instrs[*index_node].asm_instr, buf,  strlen(buf));
        (*index_node)++;

        memcpy(instrs[*index_node].asm_instr, "add r12, 8", sizeof("add r12, 8"));
        // 49 83 C4 01
        (*index_node)++;
    }
    
    return;
}

void CompleteVariable(Instruction* instrs, size_t* index_node, Node* node)
{
    if (!node) return;
    
    memcpy(instrs[*index_node].asm_instr, "mov r11, RAM_PTR", sizeof("mov r11, RAM_PTR"));
    (*index_node)++;

    char buf[20] = {};
    size_t x = 8 * node->data.id_var;
    sprintf(buf, "add r11, %lu", x);
    // 49 83 C3 xx

    memcpy(instrs[*index_node].asm_instr, buf, strlen(buf) + 1);
    (*index_node)++;

    return;
}