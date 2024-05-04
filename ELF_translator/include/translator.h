#ifndef TRANSLATORlib
#define TRANSLATORlib

#include "error_allocator.h"
#include "read_file.h"

#include <elf.h>

const size_t ALIGNMENT             = 0x1000;
const size_t TEXT_OFFSET           = 0x1000;
const size_t ABSOLUTE_FILE_ADDRESS = 0x400000; 



int     TranslateToELF(Text* elf_buf, Text* code, Text* data, err_allocator* err_alloc);
int     VerificatorELF(Text* elf_buf, err_allocator* err_alloc);

int     CompleteElfHeader(Text* elf_buf);
int     CompleteProgramHeaderTable(Text* elf_buf, Text* text, Text* data);

int     CompleteText(Text* elf_buf, Text* code);
int     CompleteData(Text* elf_buf, Text* data);


#endif