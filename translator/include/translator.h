#ifndef TRANSLATORlib
#define TRANSLATORlib

#include "error_allocator.h"
#include "read_file.h"
#include "tree.h"
//#include "../processor/spu/src/processor.h"

#include <elf.h>

int   ConvertBinary(Text* start_bin, Text* converted_bin);
int ParseInstuction(Text* start_bin, Text* converted_bin);


int  TranslateToELF(Text* elf_buf, err_allocator* err_alloc);
int     Verificator(Text* elf_buf, err_allocator* err_alloc);


int CompleteElfHeader(Text* elf_buf);
int CompleteProgramHeaderTable(Text* elf_buf);

int CompleteText(Text* elf_buf, Text* binary);
int CompleteData(Text* elf_buf);




#endif