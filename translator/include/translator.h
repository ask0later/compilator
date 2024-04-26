#ifndef TRANSLATORlib
#define TRANSLATORlib

#include "error_allocator.h"
#include "read_file.h"
#include "tree.h"
//#include "../processor/spu/src/processor.h"

#include <elf.h>

// enum Commands
// {   
//     PUSH =  1, POP  =  2,
//     ADD  =  3, SUB  =  4,
//     MUL  =  5, DIV  =  6, 
//     SQRT =  7,
//     SIN  =  8, COS  =  9,
//     IN   = 10,
//     JA   = 11, JAE  = 12, JB   = 13,
//     JBE  = 14, JE   = 15, JNE  = 16,
//     JMP  = 17,
//     CALL = 19, RETURN  = 20, 
//     OUT  = 21, HTL  = 22
// };

int   ConvertBinary(Text* start_bin, Text* converted_bin);
int ParseInstuction(Text* start_bin, Text* converted_bin);


int  TranslateToELF(Text* elf_buf, err_allocator* err_alloc);
int     Verificator(Text* elf_buf, err_allocator* err_alloc);


int CompleteElfHeader(Text* elf_buf);
int CompleteProgramHeaderTable(Text* elf_buf);

int CompleteText(Text* elf_buf, Text* binary);
int CompleteData(Text* elf_buf);




#endif