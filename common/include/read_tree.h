#ifndef READlib
#define READlib
#include "tree.h"
#include "error_allocator.h"


const size_t SIZE_FUNC_ = 5;
const size_t SIZE_VAR_  = 4;

int CreateTree(Text* buf, Tree** array_tree, size_t* num_trees, err_allocator* err_alloc);
int ReadNode(Text* buf, Tree* tree, Node** node, Order order_value, err_allocator* err_alloc);
int SkipSpaces(Text* buf);
int PasteObject(char* source, Node** node);
int ReadObject(char* source, Text* buf, err_allocator* err_alloc);

#endif