#include "tree.h"
#include "read_file.h"
#include "graphic.h"
#include "dump.h"
#include "lang.h"
#include "print_tree.h"
#include "error_allocator.h"

int main(const int argc, const char* argv[])
{
    if (argc != 3)
    {
        printf("Введите текстовый файл с программой\n");
        return 1;
    }
    
    struct err_allocator err_alloc = {};
    CtorErrorAllocator(&err_alloc);

    const char* lang_file = argv[1];
    const char* tree_file = argv[2];


    struct Text buf = {};
    struct Tree main_tree = {};

    CtorBuffer(&buf, lang_file, &err_alloc);
    
    if (err_alloc.need_call == true)
    {
        INSERT_ERROR_NODE(&err_alloc, "invalid executing CtorBuffer");
        ERR_ALLOC_TERMINATE(&err_alloc);

        DtorBuffer(&buf);
        return 1;
    }

    Tokens tkns = {};

    CtorTokens(&tkns, &buf, &err_alloc);
    if (err_alloc.need_call == true)
    {
        INSERT_ERROR_NODE(&err_alloc, "invalid executing CtorTokens");
        ERR_ALLOC_TERMINATE(&err_alloc);

        DtorTokens(&tkns);
        DtorBuffer(&buf);
        return 1;
    }
    
    CompleteTokens(&tkns, &buf, &err_alloc);
    if (err_alloc.need_call == true)
    {
        INSERT_ERROR_NODE(&err_alloc, "invalid executing CtorTokens");
        ERR_ALLOC_TERMINATE(&err_alloc);
        
        DumpErrorLocation(&tkns, &buf, lang_file);
        DtorTokens(&tkns);
        DtorBuffer(&buf);
        return 1;
    }

    Function* funcs = NULL;
    CtorFunctions(&funcs, &err_alloc);
    size_t index_func = 0;

    main_tree.root = GetGrammar(&tkns, &funcs, &index_func, &err_alloc);
    if (err_alloc.need_call == true)
    {
        INSERT_ERROR_NODE(&err_alloc, "invalid executing GetGrammar");
        ERR_ALLOC_TERMINATE(&err_alloc);

        DumpErrorLocation(&tkns, &buf, lang_file);
        DtorFunctions(&funcs);
        DtorTokens(&tkns);
        DtorBuffer(&buf);
        return 1;
    }

    if (index_func > MAX_NUM_FUNCTIONS)
    {
        INSERT_ERROR_NODE(&err_alloc, "you have exceded the max num of funcs");
        ERR_ALLOC_TERMINATE(&err_alloc);
        
        DtorFunctions(&funcs);
        DtorTokens(&tkns);
        DtorBuffer(&buf);
        return 1;
    }

    Tree* trees[MAX_NUM_FUNCTIONS] = {};

    for (size_t i = 0; i < index_func; i++)
    {
        trees[i] = &(funcs[i].tree);
    }


    FILE* output = fopen(tree_file, "w");
    if (output == NULL)
    {
        INSERT_ERROR_NODE(&err_alloc, "file opening is failed");
        ERR_ALLOC_TERMINATE(&err_alloc);
        
        DtorFunctions(&funcs);
        DtorTokens(&tkns);
        DtorBuffer(&buf);
        return 1;
    }
    PrintTrees(trees, index_func, output);

    fclose(output);
    
    GraphicDump(5, &(funcs[0].tree), &(funcs[1].tree), &(funcs[2].tree), &(funcs[3].tree), &(funcs[4].tree), &(funcs[5].tree));

    DtorFunctions(&funcs);
    DtorErrorAllocator(&err_alloc);
    DtorTokens(&tkns);
    DtorBuffer(&buf);
}