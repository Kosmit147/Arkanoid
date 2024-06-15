/// @file quad_tree.h
/// @brief A Quad Tree structure used to store the positions of blocks on the board.

#pragma once

#include <stdbool.h>

#include "entities.h"
#include "vector.h"

/// @brief Maximum number of blocks which can be store if a quad tree node.
#define MAX_QUAD_TREE_NODE_BLOCKS 5

typedef struct QuadTreeNode QuadTreeNode;

/// @brief A quad tree node.
typedef struct QuadTreeNode
{
    RectBounds bounds;
    QuadTreeNode* nodes[4];
    const Block* blocks[MAX_QUAD_TREE_NODE_BLOCKS];
} QuadTreeNode;

/// @brief Quad tree structure used to efficiently find blocks on the board within a certain. It doesn't
/// manage the objects on its own, just stores pointers to them.
typedef struct QuadTree
{
    QuadTreeNode* root;
    size_t elemCount;
} QuadTree;

/// @brief Returns whether a quad tree node has subnodes.
/// @param node Pointer to the node.
/// @return True if the node has subnodes, false otherwise.
static inline bool quadTreeNodeHasSubnodes(const QuadTreeNode* node)
{
    return node->nodes[0] != NULL;
}

/// @brief Returns whether a quad tree node is full.
/// @param node Pointer to the node.
/// @return True if the node is full, false otherwise.
static inline bool quadTreeNodeFull(const QuadTreeNode* node)
{
    return node->blocks[MAX_QUAD_TREE_NODE_BLOCKS - 1] != NULL;
}

/// @brief Creates a quad tree.
/// @param bounds The area which the quad tree will cover.
/// @return Created quad tree.
QuadTree quadTreeCreate(RectBounds bounds);
/// @brief Inserts a block pointer into the quad tree.
/// @param quadTree Pointer to the quad tree.
/// @param block Pointer to the block.
void quadTreeInsert(QuadTree* quadTree, const Block* block);
/// @brief Removes a block pointer from the quad tree.
/// @param quadTree Pointer to the quad tree.
/// @param block Pointer to the block.
void quadTreeRemoveBlock(QuadTree* quadTree, const Block* block);
/// @brief Retrieves all the blocks which at least partially cover a certain area.
/// @param quadTree Pointer to the quad tree.
/// @param bounds The area which will be searched for blocks that cover it at least partially.
/// @param block Pointer to a vector which will be filled with pointers to the retrieved blocks.
void quadTreeRetrieveAllByBounds(const QuadTree* quadTree, RectBounds bounds, Vector* result);
/// @brief Frees a quad tree object.
/// @param quadTree Pointer to the quad tree
void quadTreeFree(QuadTree* quadTree);
