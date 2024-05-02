#ifndef AST_TO_IR_lib
#define AST_TO_IR_lib

#include "error_allocator.h"
#include "tree.h"

const size_t MAX_FUNC_NUM  = 10;
const size_t MAX_NAME_SIZE = 20;

const size_t MAX_NASM_INSTR_SIZE = 30;
const size_t MAX_x64_INSTR_SIZE = 15;

const size_t MAX_LABEL_NUM = 20;

enum IR_Instruction
{
        IR_NOP          = 0,
        IR_MOV          = 1,
        IR_ADD          = 2,
        IR_SUB          = 3,
        IR_MUL          = 4,
        IR_DIV          = 5,
        IR_PUSH         = 6,
        IR_POP          = 7,
        IR_JMP          = 8,
        IR_JE           = 9,
        IR_JNE          = 10,
        IR_JAE          = 11,
        IR_JBE          = 12,
        IR_CMP          = 13,
        IR_DEFINE       = 14,
        IR_CALL         = 15,
        IR_RET          = 16,
        IR_INPUT        = 17,
        IR_OUTPUT       = 18,
        IR_LABEL        = 19
};

enum IR_type
{
        NO_ARG          = 0,
        VAR_ARG         = 1,
        NUM_ARG         = 2,
        REG_ARG         = 3,
        MEM_ARG         = 4,
        LABEL_ARG       = 5,
        STACK_ARG       = 6
};

struct IR_data
{
        int             value;
        char            name[MAX_NAME_SIZE];
};

struct IR_node
{
        IR_Instruction  instr;

        IR_type         type_1;
        IR_data         arg_1;

        IR_type         type_2;
        IR_data         arg_2;

        unsigned char   x64_instr[MAX_x64_INSTR_SIZE];
        unsigned short  x64_instr_size;

        size_t          offset;
};

struct Label
{
        char            label_name[MAX_NAME_SIZE];
        size_t          offset;
};

struct IR_Function
{
        IR_node*        instrs;
        size_t          position;
        size_t          size;

        size_t          var_num;
        size_t          if_num;
        size_t          while_num;

        size_t          func_offset;

        Label           label[MAX_LABEL_NUM];
        size_t          label_num;
};



struct args_and_opcode
{
        IR_type         type_1;
        int             arg_1;

        IR_type         type_2;
        int             arg_2;
        unsigned char   opcode[MAX_x64_INSTR_SIZE];
};

struct IR_to_opcode
{
        IR_Instruction  ir_instr;
        args_and_opcode data[30];
};


IR_to_opcode const table[] =
{
        {
        IR_MOV,
                {
                        {REG_ARG, 1, REG_ARG,  2, {0x48, 0x89, 0xC8}},
                        {REG_ARG, 1, REG_ARG,  3, {0x48, 0x89, 0xD0}},
                        {REG_ARG, 1, REG_ARG,  8, {0x4C, 0x89, 0xC0}},
                        {REG_ARG, 1, REG_ARG,  9, {0x4C, 0x89, 0xC8}},
                        {REG_ARG, 1, REG_ARG, 10, {0x4C, 0x89, 0xD0}},
                        {REG_ARG, 1, REG_ARG, 11, {0x4C, 0x89, 0xD8}}, // mov rax, r11
                        {REG_ARG, 1, REG_ARG, 12, {0x4C, 0x89, 0xE0}},
                        {REG_ARG, 1, REG_ARG, 13, {0x4C, 0x89, 0xE8}},
                        {REG_ARG, 1, REG_ARG, 14, {0x4C, 0x89, 0xF0}},
                        {REG_ARG, 1, REG_ARG, 15, {0x4C, 0x89, 0xF8}},


                       {REG_ARG, 8, MEM_ARG,  0, {0x4D, 0x8B, 0x42, 0xFF}}, // mov r8, [r10 + **]
                       {MEM_ARG, 0, REG_ARG,  1, {0x49, 0x89, 0x42, 0xFF}}, // mov [r10 + **], rax 
                       {MEM_ARG, 0, REG_ARG,  8, {0x4D, 0x89, 0x42, 0xFF}}, // mov [r10 + **], r8
                       {MEM_ARG, 0, REG_ARG,  1, {0x49, 0x89, 0x02}}, // mov [r10], rax
                       {MEM_ARG, 0, REG_ARG,  8, {0x4D, 0x89, 0x02}}  // mov [r10], r8
                }
        },
        {
        IR_PUSH,
                {
                        // push number
                        {NUM_ARG, 0, NO_ARG, 0, {0x68, 0xFF, 0xFF, 0xFF, 0xFF}},

                        {REG_ARG, 1, NO_ARG, 0, {0x50}},
                        {REG_ARG,  2, NO_ARG, 0, {0x51}},
                        {REG_ARG,  3, NO_ARG, 0, {0x52}},
                        {REG_ARG,  8, NO_ARG, 0, {0x41, 0x50}},
                        {REG_ARG,  9, NO_ARG, 0, {0x41, 0x51}},
                        {REG_ARG, 10, NO_ARG, 0, {0x41, 0x52}},
                        {REG_ARG, 11, NO_ARG, 0, {0x41, 0x53}},
                        {REG_ARG, 12, NO_ARG, 0, {0x41, 0x54}},
                        {REG_ARG, 13, NO_ARG, 0, {0x41, 0x55}},
                        {REG_ARG, 14, NO_ARG, 0, {0x41, 0x56}},
                        {REG_ARG, 15, NO_ARG, 0, {0x41, 0x57}},

                        {MEM_ARG, 0, NO_ARG, 0, {0x41, 0xFF, 0x72, 0x00}}, // push [r10 + **]
                        {MEM_ARG, 0, NO_ARG, 0, {0x41, 0xFF, 0x32}} // push [r10]
                        
                }
        },
        {
                IR_POP,
                {
                        {REG_ARG,  1, NO_ARG, 0, {0x58}},
                        {REG_ARG,  2, NO_ARG, 0, {0x59}},
                        {REG_ARG,  3, NO_ARG, 0, {0x5A}},
                        {REG_ARG,  8, NO_ARG, 0, {0x41, 0x58}},
                        {REG_ARG,  9, NO_ARG, 0, {0x41, 0x59}},
                        {REG_ARG, 10, NO_ARG, 0, {0x41, 0x5a}},
                        {REG_ARG, 11, NO_ARG, 0, {0x41, 0x5b}},
                        {REG_ARG, 12, NO_ARG, 0, {0x41, 0x5c}},
                        {REG_ARG, 13, NO_ARG, 0, {0x41, 0x5d}},
                        {REG_ARG, 14, NO_ARG, 0, {0x41, 0x5e}},
                        {REG_ARG, 15, NO_ARG, 0, {0x41, 0x5f}},
                        {MEM_ARG,  0, NO_ARG, 0, {0x41, 0x8f, 0x42, 0x00}}, // pop [r10 + **]
                        {MEM_ARG,  0, NO_ARG, 0, {0x41, 0x8f, 0x02}}
                }
        },
        {
                IR_ADD,
                {
                        {REG_ARG, 11, REG_ARG, 12, {0x4D, 0x01, 0xE3}}
                }
        },
        {
                IR_SUB,
                {
                        {REG_ARG, 11, REG_ARG, 12, {0x4D, 0x29, 0xE3}}
                }
        },
        {
                IR_MUL,
                {
                        {REG_ARG, 12, NO_ARG, 0, {0x49, 0xF7, 0xE4}}
                }
        },
        {
                IR_DIV,
                {
                        {REG_ARG, 12, NO_ARG, 0, {0x49, 0xF7, 0xF4}}
                }
        },
        {
                IR_CMP, 
                {
                        {REG_ARG, 11, REG_ARG, 12, {0x4D, 0x39, 0xE3}}
                }
        },
        {
                IR_JE,
                {
                        {LABEL_ARG, 0, NO_ARG, 0, {0x0F, 0x84, 0xFF, 0xFF, 0xFF, 0xFF}}
                }
        },
        {
                IR_JNE, 
                {
                        {LABEL_ARG, 0, NO_ARG, 0, {0x0F, 0x85, 0xFF, 0xFF, 0xFF, 0xFF}}
                }
        },
        {
                IR_JAE,
                {
                        {LABEL_ARG, 0, NO_ARG, 0, {0x0F, 0x83, 0xFF, 0xFF, 0xFF, 0xFF}}
                }
        },
        {
                IR_JBE,
                {
                        {LABEL_ARG, 0, NO_ARG, 0, {0x0F, 0x86, 0xFF, 0xFF, 0xFF, 0xFF}}
                }
        },
        {
                IR_JMP,
                {
                        {LABEL_ARG, 0, NO_ARG, 0, {0xE9, 0xFF, 0xFF, 0xFF, 0xFF}}
                }
        },
        {
                IR_CALL,
                {
                        {LABEL_ARG, 0, NO_ARG, 0, {0xE8, 0xFF, 0xFF, 0xFF, 0xFF}}
                }
        },
        {
                IR_RET,
                {
                        {NO_ARG, 0, NO_ARG, 0, {0xC3}}
                }
        }
};


//------------------------------------------------------------------------

int     CtorIR(IR_Function** funcs, err_allocator* err_alloc);
void    DtorIR(IR_Function* funcs);

int     ConvertIR(IR_Function* funcs, Tree** trees, err_allocator* err_alloc);

int     CompleteFunctionIR(IR_Function* funcs, Node* node, err_allocator* err_alloc);
int     CompletePushArgumentsIR(IR_Function* funcs, Node* node, int* arg_num, err_allocator* err_alloc);
int     CompletePopArgumentsIR(IR_Function* funcs, Node* node, err_allocator* err_alloc);

void    CompleteOperatorsIR(IR_Function* funcs, Node* node, err_allocator* err_alloc);
void    CompleteInOutPutIR(IR_Function* funcs, Node* node, err_allocator* err_alloc);

void    CompleteAssignIR(IR_Function* funcs, Node* node, err_allocator* err_alloc);
void    CompleteLoopIR(IR_Function* funcs, Node* node, err_allocator* err_alloc);
void    CompleteIfIR(IR_Function* funcs, Node* node, err_allocator* err_alloc);
void    CompleteReturnIR(IR_Function* funcs, Node* node, err_allocator* err_alloc);
void    CompleteExpressionIR(IR_Function* funcs, Node* node, err_allocator* err_alloc);

IR_Instruction CompleteBoolExpressionIR(IR_Function* funcs, Node* node, err_allocator* err_alloc);

//--------------------------------------------------------------------------

int InsertIRnode(IR_node* ir_nodes, IR_Instruction instr_id, IR_type type_1, int arg_1, char* name_1, IR_type type_2, int arg_2, char* name_2);
int InsertOpcode(IR_node* node);



int ConvertToHex(int dec_number, unsigned char* hex_number);

//--------------------------------------------------------------------------

int PatchIR(IR_Function* funcs, err_allocator* err_alloc);
int PatchJumps(IR_Function* func, Label* func_labels, size_t func_num);

size_t GetOffsetConditionalJumps(IR_node* node, IR_Function* func);
size_t GetOffsetGlobalCalls(IR_node* node, Label* func_labels, size_t func_num);

// int OptimizeIR(IR_Function* func);
// int CompareIntDescending(const void* a, const void* b);

//--------------------------------------------------------------------------

int DumpIR(IR_Function* funcs);
int DumpInstrName(IR_Instruction instr);

#endif