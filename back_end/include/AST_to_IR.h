#ifndef AST_TO_IR_lib
#define AST_TO_IR_lib

#include "error_allocator.h"
#include "tree.h"

const size_t MAX_FUNC_NUM  = 10;
const size_t MAX_NAME_SIZE = 20;

//----------------------------------------------------------

const size_t MAX_NASM_INSTR_SIZE = 30;
const size_t MAX_x64_INSTR_SIZE = 15;
const size_t MAX_LABEL_NUM = 20;

//----------------------------------------------------------

const size_t MY_INPUT_SIZE = 92;

//----------------------------------------------------------

const size_t MAX_INSTR_NAME_SIZE = 10;
const size_t MAX_REG_NAME_SIZE = 5;





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

struct IR_block
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
        IR_block*       instrs;
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
        unsigned short  opcode_size; 
};

struct IR_to_opcode
{
        IR_Instruction  ir_instr;
        args_and_opcode data[30];
};


const char* const registers[] = {"rax", "rcx", "rdx", "rbx", "rsp", "rbp", "rsi", "rdi", "r8", "r9", "r10", "r11", "r12", "r13", "r14", "r15"};

IR_to_opcode const table[] =
{
        {
        IR_MOV,
                {
                        {REG_ARG, 4, REG_ARG, 10, {0x4C, 0x89, 0xD4}, 3},  // mov rsp, r10
                        {REG_ARG,10, REG_ARG,  4, {0x49, 0x89, 0xE2}, 3},  // mov r10, rsp
                        {REG_ARG, 0, REG_ARG,  2, {0x48, 0x89, 0xC8}, 3},  // mov rax, rcx
                        {REG_ARG, 0, REG_ARG,  3, {0x48, 0x89, 0xD0}, 3},  // mov rax, rdx
                        {REG_ARG, 0, REG_ARG,  8, {0x4C, 0x89, 0xC0}, 3},  // mov rax, r8
                        {REG_ARG, 0, REG_ARG,  9, {0x4C, 0x89, 0xC8}, 3},  // mov rax, r9
                        {REG_ARG, 0, REG_ARG, 10, {0x4C, 0x89, 0xD0}, 3},  // mov rax, r10
                        {REG_ARG, 0, REG_ARG, 11, {0x4C, 0x89, 0xD8}, 3},  // mov rax, r11
                        {REG_ARG, 0, REG_ARG, 12, {0x4C, 0x89, 0xE0}, 3},  // mov rax, r12
                        {REG_ARG, 0, REG_ARG, 13, {0x4C, 0x89, 0xE8}, 3},  // mov rax, r13
                        {REG_ARG, 0, REG_ARG, 14, {0x4C, 0x89, 0xF0}, 3},  // mov rax, r14
                        {REG_ARG, 0, REG_ARG, 15, {0x4C, 0x89, 0xF8}, 3},  // mov rax, r15
                        {REG_ARG, 10, MEM_ARG, 0, {0x49, 0xBA, 0x00, 0x21, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00}, 10} // mov r10, RAM_PTR 0x402100
                }
        },
        {
        IR_PUSH,
                {
                        {NUM_ARG, 0, NO_ARG, 0, {0x68, 0xFF, 0xFF, 0xFF, 0xFF}, 5},// push 0x**

                        {REG_ARG, 0, NO_ARG, 0, {0x50}, 1},                        // push rax     
                        {REG_ARG,  1, NO_ARG, 0, {0x51}, 1},                       // push rcx
                        {REG_ARG,  2, NO_ARG, 0, {0x52}, 1},                       // push rdx
                        {REG_ARG,  8, NO_ARG, 0, {0x41, 0x50}, 2},                 // push r8
                        {REG_ARG,  9, NO_ARG, 0, {0x41, 0x51}, 2},                 // push r9
                        {REG_ARG, 10, NO_ARG, 0, {0x41, 0x52}, 2},                 // push r10
                        {REG_ARG, 11, NO_ARG, 0, {0x41, 0x53}, 2},                 // push r11
                        {REG_ARG, 12, NO_ARG, 0, {0x41, 0x54}, 2},                 // push r12
                        {REG_ARG, 13, NO_ARG, 0, {0x41, 0x55}, 2},                 // push r13
                        {REG_ARG, 14, NO_ARG, 0, {0x41, 0x56}, 2},                 // push r14
                        {REG_ARG, 15, NO_ARG, 0, {0x41, 0x57}, 2},                 // push r15

                        {MEM_ARG, 0, NO_ARG, 0, {0x41, 0xFF, 0xB2, 0xFF, 0xFF, 0xFF, 0xFF}, 7},    // push [r10 + **]
                }
        },
        {
                IR_POP,
                {
                        {REG_ARG,  0, NO_ARG, 0, {0x58}, 1},                       // pop rax               
                        {REG_ARG,  1, NO_ARG, 0, {0x59}, 1},                       // pop rcx
                        {REG_ARG,  2, NO_ARG, 0, {0x5A}, 1},                       // pop rdx
                        {REG_ARG,  7, NO_ARG, 0, {0x5F}, 1},                       // pop rdi
                        {REG_ARG,  8, NO_ARG, 0, {0x41, 0x58}, 2},                 // pop r8
                        {REG_ARG,  9, NO_ARG, 0, {0x41, 0x59}, 2},                 // pop r9
                        {REG_ARG, 10, NO_ARG, 0, {0x41, 0x5a}, 2},                 // pop r10
                        {REG_ARG, 11, NO_ARG, 0, {0x41, 0x5b}, 2},                 // pop r11
                        {REG_ARG, 12, NO_ARG, 0, {0x41, 0x5c}, 2},                 // pop r12
                        {REG_ARG, 13, NO_ARG, 0, {0x41, 0x5d}, 2},                 // pop r13
                        {REG_ARG, 14, NO_ARG, 0, {0x41, 0x5e}, 2},                 // pop r14
                        {REG_ARG, 15, NO_ARG, 0, {0x41, 0x5f}, 2},                 // pop r15

                        {MEM_ARG,  0, NO_ARG, 0, {0x41, 0x8f, 0x82, 0xFF, 0xFF, 0xFF, 0xFF}, 7},   // pop [r10 + **]
                }
        },
        {
                IR_ADD,
                {
                        {REG_ARG, 11, REG_ARG, 12, {0x4D, 0x01, 0xE3}, 3},                         // add r11, r12
                        {REG_ARG, 10, NUM_ARG,  0, {0x49, 0x81, 0xC2, 0xFF, 0xFF, 0xFF, 0xFF}, 7}, // add r10, 0x**
                        {REG_ARG,  4, NUM_ARG,  0, {0x48, 0x81, 0xC4, 0xFF, 0xFF, 0xFF, 0xFF}, 7}  // add rsp, 0x**

                }
        },
        {
                IR_SUB,
                {
                        {REG_ARG, 11, REG_ARG, 12, {0x4D, 0x29, 0xE3}, 3},                         // sub r11, r12
                        {REG_ARG, 10, NUM_ARG,  0, {0x49, 0x81, 0xEA, 0x00, 0x00, 0x00, 0x00}, 7}, // sub r10, 0x**
                        {REG_ARG,  4, NUM_ARG,  0, {0x48, 0x81, 0xEC, 0x00, 0x00, 0x00, 0x00}, 7}  // sub rsp, 0x**
                }
        },
        {
                IR_MUL,
                {
                        {REG_ARG, 12, NO_ARG, 0, {0x49, 0xF7, 0xE4}, 3}    // mul r12
                }
        },
        {
                IR_DIV,
                {
                        {REG_ARG, 12, NO_ARG, 0, {0x49, 0xF7, 0xF4}, 3}    // div r12
                }
        },
        {
                IR_CMP, 
                {
                        {REG_ARG, 11, REG_ARG, 12, {0x4D, 0x39, 0xE3}, 3}  // cmp r11, r12
                }
        },
        {
                IR_JE,
                {
                        {LABEL_ARG, 0, NO_ARG, 0, {0x0F, 0x84, 0xFF, 0xFF, 0xFF, 0xFF}, 6} // je 0x********
                }
        },
        {
                IR_JNE, 
                {
                        {LABEL_ARG, 0, NO_ARG, 0, {0x0F, 0x85, 0xFF, 0xFF, 0xFF, 0xFF}, 6} // jne 0x********
                }
        },
        {
                IR_JAE,
                {
                        {LABEL_ARG, 0, NO_ARG, 0, {0x0F, 0x83, 0xFF, 0xFF, 0xFF, 0xFF}, 6} // jae 0x********
                }
        },
        {
                IR_JBE,
                {
                        {LABEL_ARG, 0, NO_ARG, 0, {0x0F, 0x86, 0xFF, 0xFF, 0xFF, 0xFF}, 6} // jbe 0x********
                }
        },
        {
                IR_JMP,
                {
                        {LABEL_ARG, 0, NO_ARG, 0, {0xE9, 0xFF, 0xFF, 0xFF, 0xFF}, 5}       // jmp 0x********
                }
        },
        {
                IR_CALL,
                {
                        {LABEL_ARG, 0, NO_ARG, 0, {0xE8, 0xFF, 0xFF, 0xFF, 0xFF}, 5}       // call 0x********
                }
        },
        {
                IR_RET,
                {
                        {NO_ARG, 0, NO_ARG, 0, {0xC3}, 1}          // ret
                }
        },
        {
                IR_SYSCALL,
                {
                        {NO_ARG, 0, NO_ARG, 0, {0x0F, 0x05}, 2}    //syscall
                }
        }
};


//------------------------------------------------------------------------

int     CtorIR(IR_Function** ir_func, err_allocator* err_alloc);
void    DtorIR(IR_Function* ir_func);

int     ConvertIR(IR_Function* ir_funcs, Tree** trees, err_allocator* err_alloc);
int     CompleteProgramExit(IR_Function* ir_func);


int     CompleteFunctionIR(IR_Function* ir_func, Node* node, err_allocator* err_alloc);
int     CompletePushArgumentsIR(IR_Function* ir_func, Node* node, int* arg_num, err_allocator* err_alloc);
int     CompletePopArgumentsIR(IR_Function* ir_func, Node* node, err_allocator* err_alloc);

void    CompleteOperatorsIR(IR_Function* ir_func, Node* node, err_allocator* err_alloc);
void    CompleteInOutPutIR(IR_Function* ir_func, Node* node, err_allocator* err_alloc);

void    CompleteAssignIR(IR_Function* ir_func, Node* node, err_allocator* err_alloc);
void    CompleteLoopIR(IR_Function* ir_func, Node* node, err_allocator* err_alloc);
void    CompleteIfIR(IR_Function* ir_func, Node* node, err_allocator* err_alloc);
void    CompleteReturnIR(IR_Function* ir_func, Node* node, err_allocator* err_alloc);
void    CompleteExpressionIR(IR_Function* ir_func, Node* node, err_allocator* err_alloc);

IR_Instruction CompleteBoolExpressionIR(IR_Function* funcs, Node* node, err_allocator* err_alloc);

//--------------------------------------------------------------------------

int InsertIRblock(IR_Function* ir_func, IR_Instruction instr_id, IR_type type_1, int arg_1, char* name_1, IR_type type_2, int arg_2, char* name_2);
int InsertOpcode(IR_block* block);



int ConvertToHex(int dec_number, unsigned char* hex_number);

//--------------------------------------------------------------------------

int PatchIR(IR_Function* ir_func, err_allocator* err_alloc);
int PatchJumps(IR_Function* ir_func, Label* func_labels, size_t func_num, err_allocator* err_alloc);

size_t GetOffsetConditionalJumps(IR_block* ir_block, IR_Function* ir_func, err_allocator* err_alloc);
size_t GetOffsetGlobalCalls(IR_block* ir_block, Label* func_labels, size_t func_num, size_t func_offset, err_allocator* err_alloc);

//--------------------------------------------------------------------------

int DumpIR(IR_Function* ir_funcs);
int DumpInstrName(IR_Instruction ir_instr, char* instr_buf);

#endif