#include "IR_to_nasm.h"
#include "AST_to_IR.h"
#include <cstdio>

int PrintIRtoNasm(FILE* To, IR_Function* funcs)
{
        fprintf(To, "section .text\n");
        fprintf(To, "global _start\n");
        fprintf(To, "_start:\n");

        size_t func_num = 0;
        for (; func_num < MAX_FUNC_NUM && funcs[func_num].size != 0; func_num++)
                ;
        
        for (size_t func_index = 0; func_index < func_num; func_index++)
        {
                PrintIRFunction(To, funcs + func_index);
        }
        
        PrintInOutLib(To);
        
        fprintf(To, "section .data\n");
        fprintf(To, "BUF_PTR db 50 dup 0 \n");

        return 0;
}

int PrintIRFunction(FILE* To, IR_Function* funcs)
{
        IR_block* instr_ptr = funcs->instrs;

        for (size_t instr_index = 0; instr_index < funcs->size; instr_index++)
        {
                PrintIRInstruction(To, instr_ptr + instr_index);

                if(instr_ptr[instr_index].type_1 == MEM_ARG)
                {
                        fprintf(To, " QWORD ");
                }

                PrintIRArgument(To, instr_ptr[instr_index].type_1, &instr_ptr[instr_index].arg_1);

                if (instr_ptr[instr_index].instr == IR_LABEL || instr_ptr[instr_index].instr == IR_DEFINE)
                {
                        fprintf(To, ":");
                }                

                if (instr_ptr[instr_index].type_2 != NO_ARG)
                {
                        fprintf(To, ", ");
                        PrintIRArgument(To, instr_ptr[instr_index].type_2, &instr_ptr[instr_index].arg_2);
                }

                fprintf(To, "\n");
        }

        return 0;
}



int PrintIRArgument(FILE* To, IR_type type, IR_data* data)
{
        if (type == REG_ARG)
        {
                fprintf(To, "%s ", data->name);
        }
        else if (type == MEM_ARG)
        {
                fprintf(To, "%s ", data->name);
        }
        else if (type == NUM_ARG)
        {
                fprintf(To, "%d ", data->value);
        }
        else if (type == LABEL_ARG)
        {
                fprintf(To, "%s ", data->name);
        }

        return 0;
}

int PrintIRInstruction(FILE* To, IR_block* ir_block)
{
        fprintf(To, "%s ", nasm_instrs[ir_block->instr]);

        return 0;
}


int PrintInOutLib(FILE* To)
{
        fprintf(To, 
                "my_input:\n"
                                "push rdi\n"
                                "push rsi\n"
                                "push rdx\n"
                                "push r11\n"
                                "push r10\n"
                                "mov rax, 0\n"
                                "mov rdi, 1\n"
                                "mov rsi, BUF_PTR\n"
                                "mov rdx, 50\n"
                                "syscall\n"
                                "dec rax\n"
                                "mov rdi, rax\n"
                                "xor rsi, rsi\n"
                                "xor rax, rax\n"
                                "mov r10, 10\n"
                                ".loop:\n"
                                        "xor r11, r11\n"
                                        "mov r11b, byte [BUF_PTR + rsi]\n"
                                        "inc rsi\n"
                                        "sub r11b, '0'\n"
                                        "mul r10\n"
                                        "add rax, r11\n"
                                        "dec rdi\n"
                                        "cmp rdi, 0\n"
                                        "jne .loop\n"
                                "pop r10\n"
                                "pop r11\n"
                                "pop rdx\n"
                                "pop rsi\n"
                                "pop rdi\n"
                                "ret\n"
                "my_output:\n"
                                "push rax\n"
                                "push rdi\n"
                                "push rdx\n"
                                "push rsi\n"
                                "push r11\n"
                                "mov rsi, 10\n"
                                "mov r11, 49\n"
                                "mov dl, 0xa\n"
                                "mov byte [BUF_PTR + r11], dl\n"
                                "dec r11\n"
                                ".loop:\n"
                                        "xor rdx, rdx\n"
                                        "div rsi\n"
                                        "add rdx, '0'\n"
                                        "mov byte [BUF_PTR + r11], dl\n"
                                        "dec r11\n"
                                        "cmp rax, 0\n"
                                        "jne .loop\n"
                                "mov rax, 1\n"
                                "mov rdi, 1\n"
                                "mov rdx, 50\n"
                                "sub rdx, r11\n"
                                "inc r11\n"
                                "mov rsi, BUF_PTR\n" 
                                "add rsi, r11\n"
                                "syscall\n"
                                "pop r11\n"
                                "pop rsi\n"
                                "pop rdx\n"
                                "pop rdi\n"
                                "pop rax\n"
                                "ret\n");

        return 0;
}