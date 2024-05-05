#include "read_file.h"
#include "error_allocator.h"

#include "translator.h"


// ./trans ../data_and_text/text_segment.txt ../data_and_text/data_segment.txt

int main(const int argc, const char* argv[])
{
    if (argc != 3)
    {
        printf("lack of arguments\n");
        return 1;
    }

    struct err_allocator err_alloc = {};
    CtorErrorAllocator(&err_alloc);

    const char* code_file = argv[1];
    const char* data_file = argv[2];

    Text code = {};
    Text data = {};

    CtorBuffer(&code, code_file, &err_alloc);
    if (err_alloc.need_call)
    {
        INSERT_ERROR_NODE(&err_alloc, "invalid executing CtorBuffer");
        ERR_ALLOC_TERMINATE(&err_alloc);
        DtorBuffer(&code);
        return 1;
    }

    CtorBuffer(&data, data_file, &err_alloc);
    if (err_alloc.need_call)
    {
        INSERT_ERROR_NODE(&err_alloc, "invalid executing CtorBuffer");
        ERR_ALLOC_TERMINATE(&err_alloc);
        DtorBuffer(&code);
        DtorBuffer(&data);
        return 1;
    }

    Text elf_buf = {};
    CtorEmptyBuffer(&elf_buf, 20 * ALIGNMENT, &err_alloc);
    if (err_alloc.need_call)
    {
        INSERT_ERROR_NODE(&err_alloc, "invalid executing CtorEmptyBuffer");
        ERR_ALLOC_TERMINATE(&err_alloc);
        DtorBuffer(&code);
        DtorBuffer(&data);
        DtorBuffer(&elf_buf);
        return 1;
    }
    
    TranslateToELF(&elf_buf, &code, &data, &err_alloc);
    if (err_alloc.need_call)
    {
        INSERT_ERROR_NODE(&err_alloc, "invalid executing TranslateToELF");
        ERR_ALLOC_TERMINATE(&err_alloc);
        DtorBuffer(&code);
        DtorBuffer(&data);
        DtorBuffer(&elf_buf);
        return 1;
    }

    WriteFile(&elf_buf, OUTPUT_FILE, &err_alloc);
    if (err_alloc.need_call)
    {
        INSERT_ERROR_NODE(&err_alloc, "invalid executing WriteFile");
        ERR_ALLOC_TERMINATE(&err_alloc);
        DtorBuffer(&code);
        DtorBuffer(&data);
        DtorBuffer(&elf_buf);
        return 1;
    }
    
    DtorErrorAllocator(&err_alloc);
    DtorBuffer(&code);
    DtorBuffer(&data);
    DtorBuffer(&elf_buf);
}