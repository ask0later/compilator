#include "read_tree.h"
#include "operators.h"

int CreateTree(Text* buf, Tree** array_tree, size_t* num_trees, err_allocator* err_alloc)
{
    size_t i = 0;

    while (buf->position != buf->size_buffer - 1)
    {
        array_tree[i] = ConstructorTree();

        ReadNode(buf, array_tree[i], &(array_tree[i]->root), PRE_ORDER, err_alloc);
        i++;

        if (i == NUM_TREE)
        {
            err_alloc->need_call = true;
            INSERT_ERROR_NODE(err_alloc, "exceeded maximum num of trees");
            return 1;
        }
    }

    (*num_trees) = i;

    return NO_ERROR_TREE;
}

int ReadNode(Text* buf, Tree* tree, Node** node, Order order_value, err_allocator* err_alloc)
{
    SkipSpaces(buf);

    if (buf->str[buf->position] == '(')
    {
        *node = CreateNode((Type) 0, 0, NULL, NULL);

        tree->size++;
        (buf->position)++;

        char source[MAX_SIZE_NAME];
        SkipSpaces(buf);
        ReadObject(source, buf, err_alloc);

        PasteObject(source, node);

        ReadNode(buf, tree, &((*node)->left), order_value, err_alloc);
        if (err_alloc->need_call)
        {
            err_alloc->need_call = true;
            INSERT_ERROR_NODE(err_alloc, "invalid executing ReadNode");
            return 1;
        }

        ReadNode(buf, tree, &((*node)->right), order_value, err_alloc);
        if (err_alloc->need_call)
        {
            err_alloc->need_call = true;
            INSERT_ERROR_NODE(err_alloc, "invalid executing ReadNode");
            return 1;
        }

        
        SkipSpaces(buf);

        if (buf->str[buf->position] == ')')
            (buf->position)++;
    }
    else if (buf->str[buf->position] == ')')
    {
        return NO_ERROR_TREE;
    }

    return NO_ERROR_TREE;
}

int SkipSpaces(Text* buf)
{
    while (isspace(buf->str[buf->position]))
        (buf->position)++;

    return NO_ERROR_TREE;
}
    

int PasteObject(char* source, Node** node)
{
    double value = 0;

    for (size_t i = 0; i < sizeof(operators) / sizeof(Command); i++)
    {
        if (operators[i].size_name == 0)
        {
            /*DO NOTHING*/
        }
        else if (strncmp(source, operators[i].name, operators[i].size_name) == 0)
        {
            (*node)->type = OPERATOR;
            (*node)->data.id_op  = operators[i].id;
            return 0;
        }
    }

    if (sscanf(source, "%lg", &value) == 1)
    {
        (*node)->type  = NUMBER;
        (*node)->data.value = value;
    }
    else if (strncmp(source, "func_", SIZE_FUNC_) == 0)
    {
        (*node)->type = FUNCTION;
        
        size_t i = SIZE_FUNC_;
        int id = 0;
        while (source[i] != '\0')
        { 
            id = id * 10 + source[i] - '0';
            i++;
        }
        (*node)->data.id_fun = (size_t) id;
    }
    else if (strncmp(source, "var_", SIZE_VAR_) == 0)
    {
        (*node)->type = VARIABLE;

        size_t i = SIZE_VAR_;
        int id = 0;
        while (source[i] != '\0')
        { 
            id = id * 10 + source[i] - '0';
            i++;
        }
        (*node)->data.id_fun = (size_t) id;
    }

    return 0;
}


int ReadObject(char* source, Text* buf, err_allocator* err_alloc)
{
    size_t i = 0;

    while ((buf->str[buf->position] != ')') && (buf->str[buf->position] != '('))
    {
        source[i] = buf->str[buf->position];
        (buf->position)++;
        i++;
        if (i == MAX_SIZE_NAME)
        {
            err_alloc->need_call = true;
            INSERT_ERROR_NODE(err_alloc, "exceeded object max size");
            return 1;
        }
    }

    source[i] = '\0';

    return NO_ERROR_TREE;
}