#ifndef GRAPHIClib
#define GRAPHIClib

#include "stdarg.h"
#include "tree.h"

TreeError   GraphicDump(size_t num_trees, ...);
TreeError GraphicDumpTree(Tree* tree);
TreeError GraphicDumpNode(Node* node);

#endif