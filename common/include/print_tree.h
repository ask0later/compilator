#ifndef PRINTlib
#define PRINTlib

#include "tree.h"

TreeError PrintTrees(Tree** trees, size_t num_trees, FILE* To);
TreeError PrintNode(Node* node, FILE* To, Order order_value);
void PrintObject(Node* node, FILE* To);
void PrintOperator(Operators value_Operators, FILE* TO);

#endif