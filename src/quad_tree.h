#pragma once

#include "entities.h"
#include "vector.h"

#define MAX_QUAD_TREE_NODE_BLOCKS 5

typedef struct QuadTreeNode QuadTreeNode;

typedef struct QuadTreeNode
{
    unsigned int level; // is this used for anything?
    RectBounds bounds;
    QuadTreeNode* nodes[4];
    const Block* blocks[MAX_QUAD_TREE_NODE_BLOCKS];
} QuadTreeNode;

// QuadTree just stores the pointers, it doesn't manage the objects on its own
typedef struct QuadTree
{
    QuadTreeNode* root;
    size_t elemCount;
} QuadTree;

QuadTree quadTreeCreate(RectBounds bounds);
void quadTreeInsert(QuadTree* quadTree, const Block* block);
void display(const QuadTreeNode* quadTree); // TODO: remove
void quadTreeRemoveBlock(QuadTree* quadTree, const Block* block);

// Returned Vector contains pointers to retrieved blocks. Remember to free it!
Vector quadTreeRetrieveAllByBounds(const QuadTree* quadTree, RectBounds bounds);
