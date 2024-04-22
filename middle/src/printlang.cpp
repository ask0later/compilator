#include "printlang.h"
#include "operators.h"

void Retranslate(Tree** trees, size_t num_trees, FILE* To)
{
    
    for (size_t i = 0; i < num_trees; i++)
    {
        PrintFunction(trees[i]->root, To);
        fprintf(To, "\n");
    }
}

void PrintFunction(Node* node, FILE* To)
{
    if (node->type == FUNCTION)
    {
        fprintf(To, "handing over to you");
        
        fprintf(To, "|%s %s|", morse_name_func, morse_digit[node->data.id_fun].encoding);

        if (node->right == NULL)
        {
            fprintf(To, "()\n{");
            PrintOperators(node->left, To);
            fprintf(To, "};");
        }
        else
        {
            fprintf(To, "(");
            PrintArgFuncDef(node->left, To);
            fprintf(To, ")\n{");
            PrintOperators(node->right, To);
            fprintf(To, "};");
        }
    }
}

void PrintOperators(Node* node, FILE* To)
{
    /// TODO: operands[node->op].name ?
    
    if (!node) {return;}

    if (node->type == OPERATOR)
    {
        if (node->data.id_op == SEMICOLON)
        {
            PrintOperators(node->left, To);
            PrintOperators(node->right, To);
        }
        else if (node->data.id_op == OP_ASSIGN)
        {
            PrintAssign(node, To);
        }
        else if (node->data.id_op == OP_LOOP)
        {
            PrintLoop(node, To);
        }
        else if (node->data.id_op == OP_CONDITION)
        {
            PrintIf(node, To);
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
        fprintf(To, "give a signal(");
        PrintVariable(node->left, To);
        PrintVariable(node->right, To);
        fprintf(To, ");\n");
    }
    else if (id_op == OUTPUT)
    {
        fprintf(To, "get the message(");
        PrintVariable(node->left, To);
        PrintVariable(node->right, To);
        fprintf(To, ");\n");
    }
}

void PrintLoop(Node* node, FILE* To)
{
    fprintf(To, "reseption while(");
    PrintBoolExpression(node->left, To);
    fprintf(To, ")\n{");
    PrintOperators(node->right, To);
    fprintf(To, "\n};\n");
}

void PrintIf(Node* node, FILE* To)
{
    fprintf(To, "reseption if(");
    PrintBoolExpression(node->left, To);
    fprintf(To, ")\n{");
    PrintOperators(node->right, To);
    fprintf(To, "\n};\n");
}

void PrintReturn(Node* node, FILE* To)
{
    fprintf(To, "out");

    if (node->right)
        PrintExpression(node->right, To);
    else if (node->left)
        PrintExpression(node->left, To);

    fprintf(To, ";");
}

void PrintAssign(Node* node, FILE* To)
{
    PrintVariable(node->left, To);

    fprintf(To, "encoding matches");
    PrintExpression(node->right, To);

    fprintf(To, ";\n");
}

void PrintArgFuncDef(Node* node, FILE* To)
{
    if (!node) {return;}

    if (node->type == VARIABLE)
    {
        PrintVariable(node, To);
    }
    else
    {
        PrintArgFuncDef(node->left, To);
        PrintArgFuncDef(node->right, To);
    }   
}

void PrintArgFuncAnnoun(Node* node, FILE* To)
{
    if (!node) {return;}

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

    fprintf(To, "|%s ", morse_name_var);

    size_t digits_num = 0;
    int digits_array[MAX_SIZE_NAME] = {};
    
    GetDigitsArray((int) node->data.value, &digits_num, digits_array);

    for (size_t counter = digits_num; counter > 0; counter--)
    {
        fprintf(To, "%s ", morse_digit[digits_array[counter - 1]].encoding);
    }
    fprintf(To, "|");

    return;
}

void GetDigitsArray(int num, size_t* digits_num, int* digits_array)
{
    int argument_1 = 10;
    int argument_2 = 1;

    while (num > argument_1)
    {
        digits_array[(*digits_num)] = (num % argument_1) / argument_2;
        argument_1 *= 10;
        argument_2 *= 10;
        (*digits_num)++;
    }
    if ((*digits_num) == 0)
    {
        digits_array[(*digits_num)] = num;
        (*digits_num) = 1;
    }

    return;
}

void PrintBoolExpression(Node* node, FILE* To)
{
    PrintExpression(node->left, To);

    // for (size_t i = 0; i < sizeof(cmds_compare) / sizeof(Command); i++)
    // {
    //     if (cmds_compare[i].id == node->data.id_op)
    //         fprintf(To, " %s ", cmds_compare[i].name);
    // }

    PrintExpression(node->right, To);
}

void PrintExpression(Node* node, FILE* To)
{
    if (!node) {return;}

    if (node->type == OPERATOR)
    {
        PrintExpression(node->left, To);
        
        for (size_t i = 0; i < sizeof(operators) / sizeof(Command); i++)
        {
            if (operators[i].id == node->data.id_op)
                fprintf(To, " %s ", operators[i].name);
        }

        PrintExpression(node->right, To);
    }
    else if (node->type == VARIABLE)
    {
        PrintVariable(node, To);
    }
    else if (node->type == FUNCTION)
    {
        fprintf(To, "|%s %s|", morse_name_func, morse_digit[node->data.id_fun].encoding);
        fprintf(To, "(");
        PrintArgFuncAnnoun(node->left, To);
        PrintArgFuncAnnoun(node->right, To);
        fprintf(To, ")");
    }
    else if (node->type == NUMBER)
    {
        size_t digits_num = 0;
        int digits_array[MAX_SIZE_NAME] = {};
        GetDigitsArray((int) node->data.value, &digits_num, digits_array);

        fprintf(To, "|");
        for (size_t counter = digits_num; counter > 0; counter--)
        {
            fprintf(To, "%s ", morse_digit[digits_array[counter - 1]].encoding);
        }
        fprintf(To, "|");
    }
    
    return;
}