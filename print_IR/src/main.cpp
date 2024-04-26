#include "error_allocator.h"
#include "read_file.h"
#include "tree.h"
#include "graphic.h"
#include "read_tree.h"
#include "print_tree.h"


#include "print_IR.h"
#include "print_nasm.h"


int main(const int argc, const char* argv[])
{
    if (argc != 2)
    {
        printf("Введите текстовый файл с деревом\n");
        return 1;
    }

    struct err_allocator err_alloc = {};
    CtorErrorAllocator(&err_alloc);

    const char* tree_file = argv[1];
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


    Instruction* instrs = {};
    CtorInstructions(&instrs, &err_alloc);

    CompleteInstructions(&instrs, trees, &err_alloc);
    PrintInstructions(instrs);

    IR_Function* funcs = NULL;

    CtorIR(&funcs, &err_alloc);
    ConvertIR(funcs, trees, &err_alloc);
    DumpIR(funcs);
    DtorIR(funcs);

    DtorErrorAllocator(&err_alloc);
    DestructorTrees(trees, trees_num);
    DtorBuffer(&buf);
    DtorInstructions(&instrs);
}