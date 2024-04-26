#ifndef PRINT_NASM_lib
#define PRINT_NASM_lib

#include "error_allocator.h"
#include "tree.h"

// #include <elf.h>

struct Instruction
{
        char      asm_instr[30];
        unsigned int   position;

        unsigned char x86_64_instr[10];
        unsigned short      instr_size;
};


struct x86_64
{
        char name_instr[20];
        char op_code;
        size_t arg_num;
};


        // nop             90
        // 
        // add rax, 1      ;  48 83 C0 01
        // add rcx, 1      ;  48 83 C1 01
        // add r8,  1      ;  49 83 C0 01
        // add r10, 1      ;  49 83 C2 01
        // add r11, 1      ;  49 83 C3 01
        // add r12, 1      ;  49 83 C4 01
        // add r13, 1      ;  49 83 C5 01
        // add r14, 1      ;  49 83 C6 01

        // sub rax, 1      ;  48 83 E8 01
        // sub rcx, 1      ;  48 83 E9 01
        // sub rsi, 1      ;  48 83 EE 01
        // sub r10, 1      ;  49 83 EA 01
        // sub r11, 1      ;  49 83 EB 01
        // sub r12, 1      ;  49 83 EC 01
        // sub r13, 1      ;  49 83 ED 01
        // sub r14, 1      ;  49 83 EE 01

        //-----------------------------

        // add r10, 1      ; 49 83 C2 01
        // add r10, 100    ; 49 83 C2 64
        // add r10, 512    ; 49 81 C2 00 02
        // add r10, 1024   ; 49 81 C2 00 04 00 00
        // add r10, 10000  ; 49 81 C2 10 27 00 00

        //-----------------------------

        // call func_0     ; E8 11 00 00 00
        // jne end_if_0    ; 75 0C
        // je end_if_0     ; 74 09
        // jbe end_if_0    ; 76 06
        // jae end_if_0    ; 73 03
        // ret             ; C3
        

int CtorInstructions(Instruction** instrs, err_allocator* err_alloc);
void DtorInstructions(Instruction** instrs);

int PrintInstructions(Instruction* instrs);


int CompleteInstructions(Instruction** ir_nodes, Tree** trees, err_allocator* err_alloc);

int CompleteFunction(Instruction* ir_nodes, size_t* index_node, Node* node, err_allocator* err_alloc);

void CompleteArgFuncDef(Instruction* ir_nodes, size_t* index_node, Node* node);
void CompleteArgFuncAnnoun(Instruction* ir_nodes, size_t* index_node, Node* node);

void CompleteOperators(Instruction* ir_nodes, size_t* index_node, Node* node, size_t* num_while, size_t* num_if);

void CompleteInOutPut(Instruction* ir_nodes, size_t* index_node, Node* node);
void CompleteLoop(Instruction* ir_nodes, size_t* index_node, Node* node, size_t* num_while, size_t* num_if);
void CompleteIf(Instruction* ir_nodes, size_t* index_node, Node* node, size_t* num_while, size_t* num_if);
void CompleteReturn(Instruction* ir_nodes, size_t* index_node, Node* node);
void CompleteAssign(Instruction* ir_nodes, size_t* index_node, Node* node);

void CompleteBoolExpression(Instruction* ir_nodes, size_t* index_node, Node* node);
void CompleteExpression(Instruction* ir_nodes, size_t* index_node, Node* node);

void CompleteVariable(Instruction* ir_nodes, size_t* index_node, Node* node);


#endif