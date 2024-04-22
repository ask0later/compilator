#include "dump.h"
#include "print_tree.h" 
#include <cstddef>
#include <cstdio>


void DumpErrorLocation(Tokens* tkns, Text* buf, const char file[])
{
    size_t error_position = tkns->tokens[tkns->position]->text_pos;

    char* temp_buffer = (char*) calloc(buf->size_buffer + INSCRIPTION_NUM, sizeof(char));

    memcpy(temp_buffer, buf->str, error_position);

    memcpy(temp_buffer + error_position, "ERROR LOCATION>>>>> ", INSCRIPTION_NUM);

    memcpy(temp_buffer + error_position + INSCRIPTION_NUM, buf->str + error_position, buf->size_buffer - error_position);

    FILE* To = fopen(file, "w");
    fwrite(temp_buffer, buf->size_buffer + INSCRIPTION_NUM, sizeof(char), To);
    fclose(To);

    free(temp_buffer);
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
        printf("<%s>;", current->data.name);
    }
    else if (current->type == FUNCTION)
    {
        printf("FUNCTION = ");
        printf("<%s>;", current->data.name);
    }
}

