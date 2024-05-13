#include "IR_to_opcodes.h"
#include "AST_to_IR.h"
#include "error_allocator.h"
#include "read_file.h"
#include <cstddef>
#include <cstring>


int IRtoOpcode(IR_Function* funcs, Text* code, err_allocator* err_alloc)
{
        for (size_t func_index = 0; func_index < MAX_FUNC_NUM && funcs[func_index].size != 0; func_index++)
        {
                for (size_t instr_index = 0; instr_index < funcs[func_index].size; instr_index++)
                {
                        IR_Function* func = funcs + func_index;
                        TranslateInstrToOpcode(func->instrs + instr_index, code);                        
                }
        }

        AddLib(code, err_alloc);
        if (err_alloc->need_call == true)
        {
                INSERT_ERROR_NODE(err_alloc, "invalid executing AddLib");
                return 1;
        }

        code->position += (ALIGNMENT - code->position % ALIGNMENT);

        ReallocateBuffer(code, code->position, err_alloc);

        return 0;
}

int TranslateInstrToOpcode(IR_block* ir_block, Text* code)
{       
        if (ir_block->x64_instr_size)
        {
                memcpy(code->str + code->position, ir_block->x64_instr, ir_block->x64_instr_size);
                code->position += ir_block->x64_instr_size;
        }
        
        return 0;
}

int AddLib(Text* code, err_allocator* err_alloc)
{
        int error = 0;

        error += system("nasm -f elf64 ./library/lib.asm\n");
        error += system("ld -o ./library/lib ./library/lib.o\n");
        
        if (error)
        {
                INSERT_ERROR_NODE(err_alloc, "failed executing system call");
                err_alloc->need_call = true;
                return 1;
        }       

        Text exe_lib = {};
        CtorBuffer(&exe_lib, "./library/lib", err_alloc);
        if (err_alloc->need_call)
        {
                INSERT_ERROR_NODE(err_alloc, "invalid executing CtorBuffer");
                return 1;
        }

        size_t start_pos = 0, end_pos = 0;

        size_t pos = 0;
        for (;pos < exe_lib.size_buffer; pos++)
        {
                if (exe_lib.str[pos] == (char) 0x90)
                {
                        start_pos = end_pos;
                        end_pos = pos;
                }
                
                if (start_pos * end_pos != 0)
                        break;
        }

        if (pos == exe_lib.size_buffer)
        {
                INSERT_ERROR_NODE(err_alloc, "there are no borders-nop in the lib");
                err_alloc->need_call = true;
                return 1;
        }
        start_pos++;

        size_t lib_size = end_pos - start_pos + 1;

        memcpy(code->str + code->position, exe_lib.str + start_pos, lib_size);
        code->position += lib_size;

        DtorBuffer(&exe_lib);
        return 0;
}