#pragma once

#include "entities.h"
#include "vector.h"

#define MAX_QUAD_TREE_NODE_BLOCKS 5

typedef struct QuadTreeNode QuadTreeNode;

typedef struct QuadTreeNode
{
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
void quadTreeRemoveBlock(QuadTree* quadTree, const Block* block);
void quadTreeFree(QuadTree* quadTree);

// Returned Vector contains pointers to retrieved blocks. Remember to free it!
void quadTreeRetrieveAllByBounds(const QuadTree* quadTree, RectBounds bounds, Vector* result);
