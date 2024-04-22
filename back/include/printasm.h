#ifndef PRINTASMlib
#define PRINTASMlib

#include "tree.h"

void PrintToAssembler(Tree** trees, size_t num_trees, FILE* To);

void    PrintFunction(Node* node, FILE* To);
void   PrintOperators(Node* node, FILE* To, size_t* num_while, size_t* num_if);

void       PrintIf(Node* node, FILE* To, size_t* num_while, size_t* num_if);
void     PrintLoop(Node* node, FILE* To, size_t* num_while, size_t* num_if);
void   PrintAssign(Node* node, FILE* To);
void PrintInOutPut(Node* node, FILE* To);

void   PrintReturn(Node* node, FILE* To);
void PrintArgFuncDef(Node* node, FILE* To);
void PrintArgFuncAnnoun(Node* node, FILE* To);
void PrintVariable(Node* node, FILE* To);

void     PrintExpression(Node* node, FILE* To);
void PrintBoolExpression(Node* node, FILE* To);

#endif