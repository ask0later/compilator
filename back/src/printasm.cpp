#include "printasm.h"

void PrintToAssembler(Tree** trees, size_t num_trees, FILE* To)
{
    fprintf(To, "jmp fun_%lu\n", num_trees - 1);
    for (size_t i = 0; i < num_trees; i++)
    {
        PrintFunction(trees[i]->root, To);
    }
}

void PrintFunction(Node* node, FILE* To)
{
    if (node->type == FUNCTION)
    {
        fprintf(To, "fun_%lu:\n", node->data.id_fun);
        size_t num_if = 0, num_while = 0;
        if (node->right == NULL)
        {
            PrintOperators(node->left, To, &num_while, &num_if);
        }
        else
        {
            PrintArgFuncDef(node->left, To);
            PrintOperators(node->right, To, &num_while, &num_if);
        }
    }
}

void PrintOperators(Node* node, FILE* To, size_t* num_while, size_t* num_if)
{
    if (!node) {return;}

    if (node->type == OPERATOR)
    {
        if (node->data.id_op == SEMICOLON)
        {
            PrintOperators(node->left, To, num_while, num_if);
            PrintOperators(node->right, To, num_while, num_if);
        }
        else if (node->data.id_op == OP_ASSIGN)
        {
            PrintAssign(node, To);
        }
        else if (node->data.id_op == OP_LOOP)
        {
            PrintLoop(node, To, num_while, num_if);
        }
        else if (node->data.id_op == OP_CONDITION)
        {
            PrintIf(node, To, num_while, num_if);
        }
        else if (node->data.id_op == RET)
        {
            PrintReturn(node, To);
        }
        else if ((node->data.id_op == INPUT) || (node->data.id_op == OUTPUT))
        {
            PrintInOutPut(node, To);
        }
    }
    
    return;
}


void PrintInOutPut(Node* node, FILE* To)
{
    Operators id_op = node->data.id_op;

    if (id_op == INPUT)
    {
        fprintf(To, "in\n");
        fprintf(To, "pop ");
        PrintVariable(node->left, To);
        PrintVariable(node->right, To);
        fprintf(To, "\n");
    }
    else if (id_op == OUTPUT)
    {
        fprintf(To, "push ");
        PrintVariable(node->left, To);
        PrintVariable(node->right, To);
        fprintf(To, "\n");
        fprintf(To, "out\n");
    }
}

void PrintLoop(Node* node, FILE* To, size_t* num_while, size_t* num_if)
{
    size_t old_num_while = *num_while;
    (*num_while)++;
    
    fprintf(To, "while_%lu:\n", old_num_while);
    PrintBoolExpression(node->left, To);
    fprintf(To, "end_while_%lu\n", old_num_while);

    PrintOperators(node->right, To, num_while, num_if);

    fprintf(To, "jmp while_%lu\n", old_num_while);
    fprintf(To, "end_while_%lu:\n", old_num_while);

}

void PrintIf(Node* node, FILE* To, size_t* num_while, size_t* num_if)
{
    size_t old_num_if = *num_if;
    (*num_if)++;
    PrintBoolExpression(node->left, To);
    fprintf(To, "end_if_%lu\n", old_num_if);

    PrintOperators(node->right, To, num_while, num_if);

    fprintf(To, "end_if_%lu:\n", old_num_if);
}

void PrintReturn(Node* node, FILE* To)
{
    if (node->right)
        PrintExpression(node->right, To);
    else if (node->left)
        PrintExpression(node->left, To);

    fprintf(To, "ret\n");
}

void PrintAssign(Node* node, FILE* To)
{
    PrintExpression(node->right, To);

    fprintf(To, "pop ");
    PrintVariable(node->left, To);
    
    fprintf(To, "\n");
}

void PrintArgFuncDef(Node* node, FILE* To)
{
    if (!node) {return;}

    if (node->type == VARIABLE)
    {
        fprintf(To, "pop ");
        PrintVariable(node, To);
        fprintf(To, "\n");
    }
    else
    {
        PrintArgFuncDef(node->left, To);
        PrintArgFuncDef(node->right, To);
    }   
}

void PrintArgFuncAnnoun(Node* node, FILE* To)
{
    if (!node) return;

    bool is_comma = false;

    if (node->type == OPERATOR)
        if (node->data.id_op == COMMA)
            is_comma = true;
    
    if (!is_comma)
    {
        PrintExpression(node, To);
    }
    else if (is_comma)
    {
        PrintArgFuncAnnoun(node->right, To);
        PrintArgFuncAnnoun(node->left, To);
    }
}

void PrintVariable(Node* node, FILE* To)
{
    if (!node) return;
    
    fprintf(To, "[%lu]", node->data.id_var);

    return;
}
void PrintBoolExpression(Node* node, FILE* To)
{
    PrintExpression(node->left, To);
    PrintExpression(node->right, To);

    if (node->data.id_op == OP_ABOVE)
        fprintf(To, "jbe ");
    else if (node->data.id_op == OP_BELOW)
        fprintf(To, "jae ");
    else if (node->data.id_op == OP_EQUAL)
        fprintf(To, "jne ");
    else if (node->data.id_op == OP_NO_EQUAL)
        fprintf(To, "je ");
}

void PrintExpression(Node* node, FILE* To)
{
    if (!node) return;

    if (node->type == OPERATOR)
    {
        PrintExpression(node->left, To);
        PrintExpression(node->right, To);
        if (node->data.id_op == OP_ADD)
            fprintf(To, "add \n");
        else if (node->data.id_op == OP_SUB)
            fprintf(To, "sub \n");
        else if (node->data.id_op == OP_MUL)
            fprintf(To, "mul \n");
        else if (node->data.id_op == OP_DIV)
            fprintf(To, "div \n");
        else if (node->data.id_op == FUN_SIN)
            fprintf(To, "sin \n");
        else if (node->data.id_op == FUN_COS)
            fprintf(To, "cos \n");
        else if (node->data.id_op == FUN_SQRT)
            fprintf(To, "sqrt \n");
    }
    else if (node->type == VARIABLE)
    {
        fprintf(To, "push ");
        PrintVariable(node, To);
        fprintf(To, "\n");
    }
    else if (node->type == FUNCTION)
    {
        PrintArgFuncAnnoun(node->left, To);
        PrintArgFuncAnnoun(node->right, To);
        fprintf(To, "call fun_%lu\n", node->data.id_fun);
    }
    else if (node->type == NUMBER)
    {
        fprintf(To, "push ");
        fprintf(To, "%lg\n", node->data.value);
    }
    
    return;
}