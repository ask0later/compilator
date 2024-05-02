#ifndef AST_TO_NASM_lib
#define AST_TO_NASM_lib

#include "error_allocator.h"
#include "tree.h"

struct Instruction
{
        char            asm_instr[30];
        unsigned int    position;

        unsigned char x86_64_instr[10];
        unsigned short      instr_size;
};


struct x86_64
{
        char name_instr[20];
        char op_code;
        size_t arg_num;
};
        

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