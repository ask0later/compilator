#include "graphic.h"
#include "Dotter.h"
#include "operators.h"

TreeError   GraphicDump(size_t num_trees, ...)
{
    dtBegin("Tree.dot");                        // Начало dot-описания графа

    dtNodeStyle ().shape        ("box");
    dtNodeStyle ().style         ("filled");
    dtNodeStyle ().fontcolor      ("black");

    va_list one_tree;
    va_start(one_tree, num_trees);
    
    for (size_t i = 0; i < num_trees; i++)
    {
        GraphicDumpTree(va_arg(one_tree, Tree*));
    }

    va_end(one_tree);

    dtEnd();                                   // Конец dot-описания графа

    dtRender("Tree.dot", "Tree.jpg", "jpg", false);

    return NO_ERROR_TREE;
}

TreeError GraphicDumpTree(Tree* tree)
{
    if (tree)
        GraphicDumpNode(tree->root);
    
    return NO_ERROR_TREE;
}


TreeError GraphicDumpNode(Node* node)
{
    if (!node) return NO_ERROR_TREE;

    if (!node) {return NO_ERROR_TREE;}

    char str[MAX_SIZE_ARG] = {};
    if (node->type == NUMBER)
    {
        dtNodeStyle().fillcolor("#7BF2DA");//HEX_TIFFANY
        sprintf(str, "%lg", node->data.value);
    }
    else if (node->type == OPERATOR)
    {
        dtNodeStyle().fillcolor("#EE204D");//HEX_RED
        
        sprintf(str, " %s ", operators[node->data.id_op].name);
    }
    else if (node->type == VARIABLE)
    {
        dtNodeStyle().fillcolor("#21C912");//HEX_GREEN
        sprintf(str, "var_%lu", node->data.id_var);
    }
    else if (node->type == FUNCTION)
    {
        dtNodeStyle().fillcolor("#735499");//HEX_PURPLE
        sprintf(str, "func_%lu", node->data.id_fun);
    }
    
    
    dtNode((int)(size_t) node, str);

    if (node->left)
    {
        GraphicDumpNode(node->left);
        dtLink((int)(size_t) node, (int)(size_t)(node->left), "");
    }

    if (node->right)
    {
        GraphicDumpNode(node->right);
        dtLink((int)(size_t) node, (int)(size_t)(node->right), "");
    }

    return NO_ERROR_TREE;
}