#include "print_tree.h"
#include "operators.h"


TreeError PrintTrees(Tree** trees, size_t num_trees, FILE* To)
{
    for (size_t i = 0; i < num_trees; i++)
    {
        PrintNode(trees[i]->root, To, PRE_ORDER);
    }

    return NO_ERROR_TREE;
}

TreeError PrintNode(Node* node, FILE* To, Order order_value)
{
    if (!node) {return NO_ERROR_TREE;}
    if (To == NULL) {return FILE_NOT_OPEN;}
    
    TreeError error = NO_ERROR_TREE;

    fprintf(To, "(");

    if (order_value == PRE_ORDER)
        PrintObject(node, To);

    if (node->left)
    {
        error = PrintNode(node->left, To, order_value);
        if (error != NO_ERROR_TREE)
            return error;
    }
    
    if (order_value == IN_ORDER)
        PrintObject(node, To);
    
    if (node->right)
    {
        error = PrintNode(node->right, To, order_value);
        if (error != NO_ERROR_TREE)
            return error;
    }
    
    if (order_value == POST_ORDER)
        PrintObject(node, To);

    fprintf(To, ")");

    return NO_ERROR_TREE;
}

void PrintObject(Node* node, FILE* To)
{
    if (node->type == NUMBER)
        fprintf(To, "%lg", node->data.value);
    else if (node->type == OPERATOR)
    {
        PrintOperator(node->data.id_op, To);
    }
    else if (node->type == VARIABLE)
    {
        fprintf(To, "var_%lu", node->data.id_var);
    }
    else if (node->type == FUNCTION)
    {
        fprintf(To, "func_%lu", node->data.id_fun);
    }
}

void PrintOperator(Operators value_Operators, FILE* To)
{
    fprintf(To, " %s ", operators[value_Operators].name);
    return;
}