#include "dump.h"
#include "print_tree.h" 
#include <cstddef>
#include <cstdio>


void DumpErrorLocation(Tokens* tkns, Text* buf, const char file[])
{
    size_t error_position = tkns->tokens[tkns->position]->text_pos;

    size_t position = 0;
    size_t line_num = 1;
    size_t offset = 0;

    while (position != error_position)
    {
        if (buf->str[position] == '\n')
        {
            line_num++;
            offset = 0;
        }
        offset++;
        position++;
    }

    printf("%s:%lu:%lu:\n", file, line_num, offset);
    
    return;    
}

void DumpTokens(Tokens* tkns)
{
    for (size_t i = 0; i < tkns->size; i++)
    {
        printf("tokens[%2lu]: ", i);
        DumpToken(tkns->tokens[i]);
        printf("    (text_position =%lu)\n", tkns->tokens[i]->text_pos);
        printf("\n");
    }
}

void DumpToken(Node* current)
{
    if (current->type == OPERATOR)
    {
        printf("OPERATOR = ");
        PrintOperator(current->data.id_op, stdout);
    }
    else if (current->type == NUMBER)
    {
        printf("NUMBER   = ");
        printf(" %lg ", current->data.value);
    }
    else if (current->type == VARIABLE)
    {
        printf("VARIABLE = ");
        printf("<%s> ", current->data.name);
    }
    else if (current->type == FUNCTION)
    {
        printf("FUNCTION = ");
        printf("<%s>;", current->data.name);
    }
}

