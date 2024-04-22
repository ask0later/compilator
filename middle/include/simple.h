#ifndef SIMPLElib
#define SIMPLElib

#include "tree.h"

TreeError SimplificationTree(Tree* tree);
TreeError SimplificationTrees(Tree** trees, size_t num_trees);

TreeError CollapsingConstants(Tree* tree, Node** node);
TreeError RemoveDummyElements(Tree* tree, Node** node);

bool isMul1(Node* parent, Node* kid);
bool isAdd0(Node* parent, Node* kid);
bool isSub0(Node* parent, Node* kid);
bool isPow1(Node* parent, Node* kid);
bool isDiv1(Node* parent, Node* kid);
bool isMul0(Node* parent, Node* kid);
bool isDiv0(Node* parent, Node* kid);
bool isPow0(Node* parent, Node* kid);

bool isEqual(double arg1, double arg2);

#endif