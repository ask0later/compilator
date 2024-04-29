#ifndef AST_TO_IR_lib
#define AST_TO_IR_lib

#include "error_allocator.h"
#include "tree.h"

const size_t MAX_FUNC_NUM  = 10;
const size_t MAX_NAME_SIZE = 10;

enum IR_Instruction
{
        IR_NOP    = 0,
        IR_MOV    = 1,
        IR_ADD    = 2,
        IR_SUB    = 3,
        IR_MUL    = 4,
        IR_DIV    = 5,
        IR_PUSH   = 6,
        IR_POP    = 7,
        IR_JMP    = 8,
        IR_JE     = 9,
        IR_JNE    = 10,
        IR_JAE    = 11,
        IR_JBE    = 12,
        IR_CMP    = 13,
        IR_DEFINE = 14,
        IR_CALL   = 15,
        IR_RET    = 16,
        IR_INPUT  = 17,
        IR_OUTPUT = 18,
        IR_LABEL  = 19
};

enum IR_type
{
        NO_ARG    = 0,
        VAR_ARG   = 1,
        NUM_ARG   = 2,
        REG_ARG   = 3,
        MEM_ARG   = 4,
        LABEL_ARG = 5,
        STACK_ARG = 6
};

struct IR_data
{
        int     value;
        char    name[MAX_NAME_SIZE];
};

struct IR_node
{
        IR_Instruction  instr;

        IR_type         type_1;
        IR_data         arg_1;

        IR_type         type_2;
        IR_data         arg_2;
};

struct IR_Function
{
        IR_node*        instrs;
        size_t          position;
        size_t          size;

        size_t          var_num;
        size_t          if_num;
        size_t          while_num;
};

//------------------------------------------------------------------------

int     CtorIR(IR_Function** funcs, err_allocator* err_alloc);
void    DtorIR(IR_Function* funcs);

int     ConvertIR(IR_Function* funcs, Tree** trees, err_allocator* err_alloc);

int     CompleteFunctionIR(IR_Function* funcs, Node* node, err_allocator* err_alloc);
int CompletePushArgumentsIR(IR_Function* funcs, Node* node, int* arg_num);
int     CompletePopArgumentsIR(IR_Function* funcs, Node* node);

void    CompleteOperatorsIR(IR_Function* funcs, Node* node);
void    CompleteInOutPutIR(IR_Function* funcs, Node* node);

void    CompleteAssignIR(IR_Function* funcs, Node* node);
void    CompleteLoopIR(IR_Function* funcs, Node* node);
void    CompleteIfIR(IR_Function* funcs, Node* node);
void    CompleteReturnIR(IR_Function* funcs, Node* node);
void    CompleteExpressionIR(IR_Function* funcs, Node* node);

IR_Instruction CompleteBoolExpressionIR(IR_Function* funcs, Node* node);

//--------------------------------------------------------------------------

int InsertIRnode(IR_node* ir_nodes, IR_Instruction instr_id, IR_type type_1, int arg_1, char* name_1, IR_type type_2, int arg_2, char* name_2);

//--------------------------------------------------------------------------

int OptimizeIR(IR_Function* func);
int CompareIntDescending(const void* a, const void* b);

//--------------------------------------------------------------------------

int DumpIR(IR_Function* funcs);
int DumpInstrName(IR_Instruction instr);

#endif