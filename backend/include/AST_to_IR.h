#ifndef AST_TO_IR_lib
#define AST_TO_IR_lib

#include "error_allocator.h"
#include "tree.h"

const size_t MAX_FUNC_NUM  = 10;
const size_t MAX_NAME_SIZE = 20;

const size_t MAX_NASM_INSTR_SIZE = 30;
const size_t MAX_x64_INSTR_SIZE = 15;

const size_t MAX_LABEL_NUM = 20;

const size_t MY_INPUT_SIZE = 92;

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
        IR_LABEL        = 19,
        IR_SYSCALL      = 20
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

        size_t          bytes;
        size_t          offset_from_start;

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


const char* const registers[] = {"", "rax", "rcx", "rdx", "rbx", "rbp", "rsi", "rdi", "r8", "r9", "r10", "r11", "r12", "r13", "r14", "r15"};

IR_to_opcode const table[] =
{
        {
        IR_MOV,
                {
                        {REG_ARG, 1, REG_ARG,  2, {0x48, 0x89, 0xC8}},  // mov rax, rcx
                        {REG_ARG, 1, REG_ARG,  3, {0x48, 0x89, 0xD0}},  // mov rax, rdx
                        {REG_ARG, 1, REG_ARG,  8, {0x4C, 0x89, 0xC0}},  // mov rax, r8
                        {REG_ARG, 1, REG_ARG,  9, {0x4C, 0x89, 0xC8}},  // mov rax, r9
                        {REG_ARG, 1, REG_ARG, 10, {0x4C, 0x89, 0xD0}},  // mov rax, r10
                        {REG_ARG, 1, REG_ARG, 11, {0x4C, 0x89, 0xD8}},  // mov rax, r11
                        {REG_ARG, 1, REG_ARG, 12, {0x4C, 0x89, 0xE0}},  // mov rax, r12
                        {REG_ARG, 1, REG_ARG, 13, {0x4C, 0x89, 0xE8}},  // mov rax, r13
                        {REG_ARG, 1, REG_ARG, 14, {0x4C, 0x89, 0xF0}},  // mov rax, r14
                        {REG_ARG, 1, REG_ARG, 15, {0x4C, 0x89, 0xF8}},  // mov rax, r15
                }
        },
        {
        IR_PUSH,
                {
                        {NUM_ARG, 0, NO_ARG, 0, {0x68, 0xFF, 0xFF, 0xFF, 0xFF}},// push 0x**

                        {REG_ARG, 1, NO_ARG, 0, {0x50}},                        // push rax     
                        {REG_ARG,  2, NO_ARG, 0, {0x51}},                       // push rcx
                        {REG_ARG,  3, NO_ARG, 0, {0x52}},                       // push rdx
                        {REG_ARG,  8, NO_ARG, 0, {0x41, 0x50}},                 // push r8
                        {REG_ARG,  9, NO_ARG, 0, {0x41, 0x51}},                 // push r9
                        {REG_ARG, 10, NO_ARG, 0, {0x41, 0x52}},                 // push r10
                        {REG_ARG, 11, NO_ARG, 0, {0x41, 0x53}},                 // push r11
                        {REG_ARG, 12, NO_ARG, 0, {0x41, 0x54}},                 // push r12
                        {REG_ARG, 13, NO_ARG, 0, {0x41, 0x55}},                 // push r13
                        {REG_ARG, 14, NO_ARG, 0, {0x41, 0x56}},                 // push r14
                        {REG_ARG, 15, NO_ARG, 0, {0x41, 0x57}},                 // push r15

                        {MEM_ARG, 0, NO_ARG, 0, {0x41, 0xFF, 0xB2, 0xFF, 0xFF, 0xFF, 0xFF}},    // push [r10 + **]
                }
        },
        {
                IR_POP,
                {
                        {REG_ARG,  1, NO_ARG, 0, {0x58}},                       // pop rax               
                        {REG_ARG,  2, NO_ARG, 0, {0x59}},                       // pop rcx
                        {REG_ARG,  3, NO_ARG, 0, {0x5A}},                       // pop rdx
                        {REG_ARG,  7, NO_ARG, 0, {0x5F}},                       // pop rdi
                        {REG_ARG,  8, NO_ARG, 0, {0x41, 0x58}},                 // pop r8
                        {REG_ARG,  9, NO_ARG, 0, {0x41, 0x59}},                 // pop r9
                        {REG_ARG, 10, NO_ARG, 0, {0x41, 0x5a}},                 // pop r10
                        {REG_ARG, 11, NO_ARG, 0, {0x41, 0x5b}},                 // pop r11
                        {REG_ARG, 12, NO_ARG, 0, {0x41, 0x5c}},                 // pop r12
                        {REG_ARG, 13, NO_ARG, 0, {0x41, 0x5d}},                 // pop r13
                        {REG_ARG, 14, NO_ARG, 0, {0x41, 0x5e}},                 // pop r14
                        {REG_ARG, 15, NO_ARG, 0, {0x41, 0x5f}},                 // pop r15

                        {MEM_ARG,  0, NO_ARG, 0, {0x41, 0x8f, 0x82, 0xFF, 0xFF, 0xFF, 0xFF}},   // pop [r10 + **]
                }
        },
        {
                IR_ADD,
                {
                        {REG_ARG, 11, REG_ARG, 12, {0x4D, 0x01, 0xE3}},                         // add r11, r12
                        {REG_ARG, 10, NUM_ARG,  0, {0x49, 0x81, 0xC2, 0xFF, 0xFF, 0xFF, 0xFF}}  // add r10, 0x**
                }
        },
        {
                IR_SUB,
                {
                        {REG_ARG, 11, REG_ARG, 12, {0x4D, 0x29, 0xE3}},                         // sub r11, r12
                        {REG_ARG, 10, NUM_ARG,  0, {0x49, 0x81, 0xEA, 0xFF, 0xFF, 0xFF, 0xFF}}  // sub r10, 0x**
                }
        },
        {
                IR_MUL,
                {
                        {REG_ARG, 12, NO_ARG, 0, {0x49, 0xF7, 0xE4}}    // mul r12
                }
        },
        {
                IR_DIV,
                {
                        {REG_ARG, 12, NO_ARG, 0, {0x49, 0xF7, 0xF4}}    // div r12
                }
        },
        {
                IR_CMP, 
                {
                        {REG_ARG, 11, REG_ARG, 12, {0x4D, 0x39, 0xE3}}  // cmp r11, r12
                }
        },
        {
                IR_JE,
                {
                        {LABEL_ARG, 0, NO_ARG, 0, {0x0F, 0x84, 0xFF, 0xFF, 0xFF, 0xFF}} // je 0x********
                }
        },
        {
                IR_JNE, 
                {
                        {LABEL_ARG, 0, NO_ARG, 0, {0x0F, 0x85, 0xFF, 0xFF, 0xFF, 0xFF}} // jne 0x********
                }
        },
        {
                IR_JAE,
                {
                        {LABEL_ARG, 0, NO_ARG, 0, {0x0F, 0x83, 0xFF, 0xFF, 0xFF, 0xFF}} // jae 0x********
                }
        },
        {
                IR_JBE,
                {
                        {LABEL_ARG, 0, NO_ARG, 0, {0x0F, 0x86, 0xFF, 0xFF, 0xFF, 0xFF}} // jbe 0x********
                }
        },
        {
                IR_JMP,
                {
                        {LABEL_ARG, 0, NO_ARG, 0, {0xE9, 0xFF, 0xFF, 0xFF, 0xFF}}       // jmp 0x********
                }
        },
        {
                IR_CALL,
                {
                        {LABEL_ARG, 0, NO_ARG, 0, {0xE8, 0xFF, 0xFF, 0xFF, 0xFF}}       // call 0x********
                }
        },
        {
                IR_RET,
                {
                        {NO_ARG, 0, NO_ARG, 0, {0xC3}}          // ret
                }
        },
        {
                IR_SYSCALL,
                {
                        {NO_ARG, 0, NO_ARG, 0, {0x0F, 0x05}}    //syscall
                }
        }
};


//------------------------------------------------------------------------

int     CtorIR(IR_Function** funcs, err_allocator* err_alloc);
void    DtorIR(IR_Function* funcs);

int     ConvertIR(IR_Function* funcs, Tree** trees, err_allocator* err_alloc);
int     CompleteProgramExit(IR_Function* func);


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
int PatchJumps(IR_Function* func, Label* func_labels, size_t func_num, err_allocator* err_alloc);

size_t GetOffsetConditionalJumps(IR_node* node, IR_Function* func, err_allocator* err_alloc);
size_t GetOffsetGlobalCalls(IR_node* node, Label* func_labels, size_t func_num, size_t func_offset, err_allocator* err_alloc);

// int OptimizeIR(IR_Function* func);
// int CompareIntDescending(const void* a, const void* b);

//--------------------------------------------------------------------------

int DumpIR(IR_Function* funcs);
int DumpInstrName(IR_Instruction instr);

#endif