#ifndef PRINTLANGlib
#define PRINTLANGlib

#include "operators.h"
#include "tree.h"
#include "lang.h"

const char morse_name_func[] = "..-. ..- -. -.-.";
const char  morse_name_var[] = "...- .- .-.";

void Retranslate(Tree** trees, size_t num_trees, FILE* To);
void GetDigitsArray(int num, size_t* digits_num, int* digits_array);


void    PrintFunction(Node* node, FILE* To);
void   PrintOperators(Node* node, FILE* To);

void          PrintIf(Node* node, FILE* To);
void        PrintLoop(Node* node, FILE* To);
void      PrintAssign(Node* node, FILE* To);
void    PrintInOutPut(Node* node, FILE* To);

void      PrintReturn(Node* node, FILE* To);

void  PrintArgFuncDef(Node* node, FILE* To);
void PrintArgFuncAnnoun(Node* node, FILE* To);
void PrintVariable(Node* node, FILE* To);

void     PrintExpression(Node* node, FILE* To);
void PrintBoolExpression(Node* node, FILE* To);

#endif