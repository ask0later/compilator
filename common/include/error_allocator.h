#ifndef error_allocatorlib
#define error_allocatorlib

#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>

#define INSERT_ERROR_NODE(err_alloc, message) ErrorInsert(err_alloc, message, __FILE__, __FUNCTION__, __LINE__)
#define ERR_ALLOC_TERMINATE(err_alloc) DumpError(err_alloc); DtorErrorAllocator(err_alloc)

const size_t MAX_SIZE_MESSAGE  = 40;
const size_t NUMBERS_OF_ERRORS = 3;

struct err_info
{
    int     code;
    int     smth_data;
    char    message[MAX_SIZE_MESSAGE];
    char    file[MAX_SIZE_MESSAGE]; 
    char    func[MAX_SIZE_MESSAGE];   
    int     line;
};


struct err_node
{
    ssize_t      next;
    err_info    error;
};

struct err_allocator
{
    bool        need_call;
    size_t      size;
    err_node*   nodes;
};

int CtorErrorAllocator(err_allocator* err_alloc);
int DtorErrorAllocator(err_allocator* err_alloc);

int ErrorInsert(err_allocator* err_alloc, const char message[], const char file[], const char func[], const int line);
int ErrorErase(err_allocator* err_alloc);


int DumpError(err_allocator* err_alloc);

#endif