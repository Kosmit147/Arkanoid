#pragma once

#include <stdbool.h>

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

static inline bool quadTreeNodeHasSubnodes(const QuadTreeNode* node)
{
    return node->nodes[0] != NULL;
}

static inline bool quadTreeNodeFull(const QuadTreeNode* node)
{
    return node->blocks[MAX_QUAD_TREE_NODE_BLOCKS - 1] != NULL;
}

QuadTree quadTreeCreate(RectBounds bounds);
void quadTreeInsert(QuadTree* quadTree, const Block* block);
void quadTreeRemoveBlock(QuadTree* quadTree, const Block* block);
void quadTreeRetrieveAllByBounds(const QuadTree* quadTree, RectBounds bounds, Vector* result);
void quadTreeFree(QuadTree* quadTree);
