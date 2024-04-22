#include "error_allocator.h"


int CtorErrorAllocator(err_allocator* err_alloc)
{
    err_node* nodes = NULL;

    nodes = (err_node*) calloc(NUMBERS_OF_ERRORS, sizeof(err_node));
    if (!nodes) return 1;
    nodes->next = -1;                   // next doesn't exist

    err_alloc->size  = 0;
    err_alloc->need_call  = false;
    err_alloc->nodes = nodes;

    return 0;
}

int DtorErrorAllocator(err_allocator* err_alloc)
{
    err_alloc->size = __INT_MAX__;;
    free(err_alloc->nodes);

    return 0;
}


int ErrorInsert(err_allocator* err_alloc, const char message[], const char file[], const char func[], const int line)
{
    assert(err_alloc);

    err_node* current_node = &(err_alloc->nodes[err_alloc->size]);
    
    current_node->error.code = 0;

    strcpy(current_node->error.message, message);
    strcpy(current_node->error.file, file);
    strcpy(current_node->error.func, func);
    current_node->error.line = line;

    
    if (err_alloc->size == NUMBERS_OF_ERRORS - 1)
    {
        size_t next = err_alloc->size;

        err_alloc->size = 0;

        err_alloc->nodes[0].next = (ssize_t)next;
        
        return 0;
    }
    
    err_alloc->nodes[err_alloc->size + 1].next = (ssize_t) err_alloc->size;
    
    err_alloc->size++;

    return 0;
}

int ErrorErase(err_allocator* err_alloc)
{
    assert(err_alloc);

    err_node* current_node = &(err_alloc->nodes[err_alloc->size]);

    if (current_node->next == -1)
        return 1;

    err_alloc->size = (size_t) current_node->next;
    
    current_node->error.code = 0;
    memset(current_node->error.message, 0, MAX_SIZE_MESSAGE);
    memset(current_node->error.func, 0, MAX_SIZE_MESSAGE);
    memset(current_node->error.file, 0, MAX_SIZE_MESSAGE);

    err_alloc->nodes[err_alloc->size].error.line = 0;



    return 0 ;
}

int DumpError(err_allocator* err_alloc)
{
    assert(err_alloc);
    
    printf("<<error allocator find error>>\n");
    char* message = NULL;
    char* file = NULL;
    char* func = NULL;
    int line = 0;

    ssize_t next = err_alloc->nodes[err_alloc->size].next;

    size_t counter = 0;

    while (next != -1 && counter < NUMBERS_OF_ERRORS)
    {
        counter++;
        printf("(%lu) ", counter);

        message = err_alloc->nodes[next].error.message;
        file    = err_alloc->nodes[next].error.file;
        func    = err_alloc->nodes[next].error.func;
        line    = err_alloc->nodes[next].error.line;
        
        next = err_alloc->nodes[next].next;

        printf("%s: call %s from %s:%d\n", message, func, file, line);
    }

    return 0;
}
