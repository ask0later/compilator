#include "error_allocator.h"
#include "read_file.h"
#include "tree.h"
#include "graphic.h"
#include "read_tree.h"
#include "print_tree.h"


#include "AST_to_IR.h"
#include "IR_to_nasm.h"
#include "AST_to_nasm.h"

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


    // Instruction* instrs = {};
    // CtorInstructions(&instrs, &err_alloc);
    // CompleteInstructions(&instrs, trees, &err_alloc);
    // PrintInstructions(instrs);
    // DtorInstructions(&instrs);

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
    ConvertIR(funcs, trees, &err_alloc);
    DumpIR(funcs);



    PrintIRtoNasm(To, funcs);
    

    DtorIR(funcs);
    DtorBuffer(&buf);
    DestructorTrees(trees, trees_num);
    DtorErrorAllocator(&err_alloc);
}