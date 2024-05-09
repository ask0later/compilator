#include "error_allocator.h"
#include "read_file.h"
#include "tree.h"
#include "graphic.h"
#include "read_tree.h"
#include "print_tree.h"


#include "AST_to_IR.h"
#include "IR_to_nasm.h"
#include "IR_to_opcodes.h"

const char TEXT_SEGMENT_FILE[] = "../data_and_text/text_segment.txt";
const char DATA_SEGMENT_FILE[] = "../data_and_text/data_segment.txt";

// ./ir ../examples/tree_factorial.txt nasm_file.asm ../asm_hex_dump/dump_factorial.txt

// nasm -f elf64 nasm_file.asm 
// ld -o test nasm_file.o

int main(const int argc, const char* argv[])
{
    if (argc != 3)
    {
        printf("lack of arguments\n");
        return 1;
    }
    
    const char* tree_file = argv[1];
    const char* nasm_file = argv[2];

    struct err_allocator err_alloc = {};
    CtorErrorAllocator(&err_alloc);

    struct Text buf = {};

    CtorBuffer(&buf, tree_file, &err_alloc);
    if (err_alloc.need_call == true)
    {
        INSERT_ERROR_NODE(&err_alloc, "file failed to open");
        ERR_ALLOC_TERMINATE(&err_alloc);
        DtorBuffer(&buf);
        return 1;
    }

    Tree* trees[NUM_TREE] = {};
    size_t trees_num = 0;

    CreateTree(&buf, trees, &trees_num, &err_alloc);
    if (err_alloc.need_call == true)
    {
        INSERT_ERROR_NODE(&err_alloc, "invalid executing CreateTree");
        ERR_ALLOC_TERMINATE(&err_alloc);
        DestructorTrees(trees, trees_num);
        DtorBuffer(&buf);
        return 1;
    }

    GraphicDump(trees_num, trees[0], trees[1], trees[2], trees[3], trees[4]);

    FILE* To = fopen(nasm_file, "w");
    if (!To)
    {
        INSERT_ERROR_NODE(&err_alloc, "file opening is failed");
        ERR_ALLOC_TERMINATE(&err_alloc);
        
        DtorBuffer(&buf);
        DestructorTrees(trees, trees_num);
        DtorErrorAllocator(&err_alloc);
        return 1;
    }


    IR_Function* funcs = NULL;
    CtorIR(&funcs, &err_alloc);
    if (err_alloc.need_call)
    {
        INSERT_ERROR_NODE(&err_alloc, "invalid executing CtorIR");
        ERR_ALLOC_TERMINATE(&err_alloc);
        
        DtorIR(funcs);
        DtorBuffer(&buf);
        DestructorTrees(trees, trees_num);
        DtorErrorAllocator(&err_alloc);
        return 1;
    }

    ConvertIR(funcs, trees, &err_alloc);
    if (err_alloc.need_call)
    {
        INSERT_ERROR_NODE(&err_alloc, "invalid executing ConvertIR");
        ERR_ALLOC_TERMINATE(&err_alloc);
        
        DtorIR(funcs);
        DtorBuffer(&buf);
        DestructorTrees(trees, trees_num);
        DtorErrorAllocator(&err_alloc);
        return 1;
    }

    //DumpIR(funcs);
    PrintIRtoNasm(To, funcs);


    Segments segment = {};
    CtorEmptyBuffer(&segment.text, 4 * ALIGNMENT, &err_alloc);
    CtorEmptyBuffer(&segment.data, ALIGNMENT, &err_alloc);

    IRtoOpcode(funcs, &segment.text, &err_alloc);
    if (err_alloc.need_call)
    {
        INSERT_ERROR_NODE(&err_alloc, "invalid executing ConvertIR");
        ERR_ALLOC_TERMINATE(&err_alloc);
        
        DtorIR(funcs);
        DtorBuffer(&buf);
        DtorBuffer(&segment.text);
        DtorBuffer(&segment.data);
        DestructorTrees(trees, trees_num);
        DtorErrorAllocator(&err_alloc);
        return 1;
    }

    WriteFile(&segment.text, TEXT_SEGMENT_FILE, &err_alloc);
    WriteFile(&segment.data, DATA_SEGMENT_FILE, &err_alloc);

    DtorIR(funcs);
    DtorBuffer(&buf);
    DtorBuffer(&segment.text);
    DtorBuffer(&segment.data);
    DestructorTrees(trees, trees_num);
    DtorErrorAllocator(&err_alloc);
}