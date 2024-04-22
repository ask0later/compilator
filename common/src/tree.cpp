#include "tree.h"

Tree* ConstructorTree()
{
    Tree* new_tree = (Tree*) calloc(1, sizeof(Tree));
    new_tree->root = NULL;
    new_tree->size = 0;
    return new_tree;
}

void DestructorTree(Tree* tree)
{
    DeleteNode(tree->root);
    tree->size = INT_MAX;
    free(tree);
}

void DestructorTrees(Tree** trees, size_t num_trees)
{
    for (size_t i = 0; i < num_trees; i++)
    {
        DestructorTree(trees[i]);
    }
}

Node* CreateVariable(size_t id_var, char* name, Node* left, Node* right)
{
    Node* node = (Node*) calloc(1, sizeof(Node));
    if (!node) {return 0;}

    node->left   = left;
    node->right  = right;

    node->type = VARIABLE;

    if (name != 0)
        memcpy(node->data.name, name, strlen(name));
    else
        node->data.id_var = id_var;

    return node;
}

Node* CreateFunction(size_t id_fun, char* name, Node* left, Node* right)
{
    Node* node = (Node*) calloc(1, sizeof(Node));
    if (!node) {return 0;}

    node->left   = left;
    node->right  = right;

    node->type = FUNCTION;

    if (name != 0)
        memcpy(node->data.name, name, strlen(name));
    else
        node->data.id_fun = id_fun;

    return node;
}

Node* CreateNumber(double value, Node* left, Node* right)
{
    Node* node = (Node*) calloc(1, sizeof(Node));
    if (!node) {return 0;}

    node->left   = left;
    node->right  = right;

    node->type = NUMBER;

    node->data.value = value;

    return node;
}

Node* CreateOperator(Operators value, Node* left, Node* right)
{
    Node* node = (Node*) calloc(1, sizeof(Node));
    if (!node) {return 0;}

    node->left   = left;
    node->right  = right;

    node->type = OPERATOR;

    node->data.id_op = value;

    return node;
}

Node* CreateNode(Type type, void* value, Node* left, Node* right)
{
    Node* node = (Node*) calloc(1, sizeof(Node));
    if (!node) {return 0;}

    node->left   = left;
    node->right  = right;

    node->type = type;


    switch (node->type)
    {
    case NUMBER:
        node->data.value = *((double*) value);
        break;
    case OPERATOR:
        node->data.id_op  = *((Operators*) value);
        break;
    case VARIABLE:
        break;
    case FUNCTION:
    default:
        break;
    }

    return node;
}

void DeleteNode(Node* node)
{
    if (!node) return;

    if (node->left)
        DeleteNode(node->left);
    if (node->right)
        DeleteNode(node->right);
    
    free(node);
    
    return;
}


Node* Copy(Node* node)
{
    if (!node) return NULL;

    Node* new_node;

    if (node->type == NUMBER)
        new_node = CreateNumber(node->data.value, Copy(node->left), Copy(node->right));
    else if (node->type == OPERATOR)
        new_node = CreateOperator(node->data.id_op, Copy(node->left), Copy(node->right));
    else if (node->type == VARIABLE)
        new_node = CreateVariable(node->data.id_var, 0, Copy(node->left), Copy(node->right));
    else if (node->type == FUNCTION)
        new_node = CreateVariable(node->data.id_fun, 0, Copy(node->left), Copy(node->right));    

    return new_node;
}