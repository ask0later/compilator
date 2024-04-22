#include "simple.h"

bool isMul1(Node* parent, Node* kid)
{
    if (!kid) return false;
    if (kid->type != NUMBER) return false;
    
    if ((parent->data.id_op == OP_MUL) && (isEqual(kid->data.value, 1)))
        return true;
    return false;
}
bool isAdd0(Node* parent, Node* kid)
{
    if (!kid) return false;
    if (kid->type != NUMBER) return false;

    if ((parent->data.id_op == OP_ADD) && (isEqual(kid->data.value, 0)))
        return true;
    return false;
}
bool isSub0(Node* parent, Node* kid)
{
    if (!kid) return false;
    if (kid->type != NUMBER) return false;

    if ((parent->data.id_op == OP_SUB) && (isEqual(kid->data.value, 0)))
        return true;
    return false;
}
bool isPow1(Node* parent, Node* kid)
{
    if (!kid) return false;
    if (kid->type != NUMBER) return false;

    if ((parent->data.id_op == FUN_POW) && (isEqual(kid->data.value, 1)))
        return true;
    return false;
}
bool isDiv1(Node* parent, Node* kid)
{
    if (!kid) return false;
    if (kid->type != NUMBER) return false;

    if ((parent->data.id_op == OP_DIV) && (isEqual(kid->data.value, 1)))
        return true;
    return false;
}
bool isMul0(Node* parent, Node* kid)
{
    if (!kid) return false;
    if (kid->type != NUMBER) return false;

    if ((parent->data.id_op == OP_MUL) && (isEqual(kid->data.value, 0)))
        return true;
    return false;
}
bool isDiv0(Node* parent, Node* kid)
{
    if (!kid) return false;
    if (kid->type != NUMBER) return false;

    if ((parent->data.id_op == OP_DIV) && (isEqual(kid->data.value, 0))) // only for node->left
        return true;
    return false;
}
bool isPow0(Node* parent, Node* kid)
{
    if (!kid) return false;
    if (kid->type != NUMBER) return false;

    if ((parent->data.id_op == FUN_POW) && (isEqual(kid->data.value, 0)))
        return true;
    return false;
}

bool isEqual(double arg1, double arg2)
{
    double modul = fabs(fabs(arg1) - fabs(arg2));

    if (modul < DBL_EPSILON)
        return true;
    else
        return false;

}


TreeError RemoveDummyElements(Tree* tree, Node** node)
{
    if (!(*node)) {return NO_ERROR_TREE;}
    
    Node** left  = &(*node)->left;
    Node** right = &(*node)->right;

    Node* copy_node = {};
    bool check = false;

    

    if ((*node)->type == OPERATOR)
    {
        check |= isMul1(*node, *right);
        check |= isAdd0(*node, *right);
        check |= isSub0(*node, *right);

        check |= isDiv1(*node, *right);

        check |= isPow1(*node, *right);

        if (check == true)
        {
            copy_node = Copy((*node)->left);
            tree->changes_num++;
            DeleteNode(*node);
            *node = copy_node;
            
            return NO_ERROR_TREE;
        }

        check |= isMul0(*node, *right);

        if (check == true)
        {
            tree->changes_num++;
            DeleteNode(*node);
            *node = CreateNumber(0, NULL, NULL);
            
            return NO_ERROR_TREE;
        }

        check |= isMul1(*node, *left);
        check |= isAdd0(*node, *left);

        if (check == true)
        {
            copy_node = Copy((*node)->right);
            tree->changes_num++;
            DeleteNode(*node);
            *node = copy_node;
            
            return NO_ERROR_TREE;
        }

        check |= isMul0(*node, *left);
        check |= isDiv0(*node, *left);

        if (check == true)
        {
            tree->changes_num++;
            DeleteNode(*node);
            *node = CreateNumber(0, NULL, NULL);

            return NO_ERROR_TREE;
        }
    }
    
    RemoveDummyElements(tree, left);
    RemoveDummyElements(tree, right);
    
    return NO_ERROR_TREE;
}

TreeError CollapsingConstants(Tree* tree, Node** node)
{
    if (!(*node)) {return NO_ERROR_TREE;}

    Node** left  = &(*node)->left;
    Node** right = &(*node)->right;

    if (((*node)->left == NULL) || ((*node)->right == NULL))
        return NO_ERROR_TREE;

    if ((*node)->type == OPERATOR)
    {
        if (((*left)->type == NUMBER) && ((*right)->type == NUMBER))
        {
            tree->changes_num++;
            double value = 0, value_left  = (*left)->data.value,\
                            value_right = (*right)->data.value;

            if ((*node)->data.id_op == OP_ADD)
                value = value_left + value_right;

            if ((*node)->data.id_op == OP_SUB)
                value = value_left - value_right;
            
            if ((*node)->data.id_op == OP_MUL)
                value = value_left * value_right;
            
            if ((*node)->data.id_op == OP_DIV)
            {

                value = value_left / value_right;
            }
    
            DeleteNode(*node);
            *node = CreateNumber(value, NULL, NULL);
            return NO_ERROR_TREE;
        }
        else
        {
            CollapsingConstants(tree, left);
            CollapsingConstants(tree, right);
        }
    }
    else
    {
        CollapsingConstants(tree, left);
        CollapsingConstants(tree, right);
    }

    return NO_ERROR_TREE;
}

TreeError SimplificationTree(Tree* tree)
{
    size_t start_changes_num = 0;
    do
    {
        start_changes_num = tree->changes_num;
        RemoveDummyElements(tree, &(tree->root));
        CollapsingConstants(tree, &(tree->root));
        
    } while (start_changes_num != tree->changes_num);

    return NO_ERROR_TREE;
}

TreeError SimplificationTrees(Tree** trees, size_t num_trees)
{
    for (size_t i = 0; i < num_trees; i++)
    {
        SimplificationTree(trees[i]);
    }

    return NO_ERROR_TREE;
}