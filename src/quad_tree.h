#pragma once

#include "entities.h"

#define MAX_OBJECTS 10
#define INVALID_QUADRANT_INDEX SIZE_MAX

#define TOP_LEFT_QUADRANT_INDEX 0
#define TOP_RIGHT_QUADRANT_INDEX 1
#define BOTTOM_RIGHT_QUADRANT_INDEX 2
#define BOTTOM_LEFT_QUADRANT_INDEX 3

typedef struct QuadTreeNode
{
    int level;
    Rect bounds;
    struct QuadTreeNode* nodes[4];
    void* objects[MAX_OBJECTS];
    size_t objCount;
} QuadTreeNode;

QuadTreeNode* createQuadTree(int level, Rect bounds);
void split(QuadTreeNode* quadTree);
size_t getIndex(Rect bounds, const Block* object);
void insert(QuadTreeNode* quadTree, const Block* object);
void retrieve(const QuadTreeNode* quadTree, const Block* object);
Block* retrieveNth(const QuadTreeNode* quadTree, size_t index);
void display(const QuadTreeNode* quadTree);
void removeBlock(QuadTreeNode* quadTree, const Block* block);
void retrieveBlocks(const QuadTreeNode* quadTree, Vec2 position, Block** blocks, size_t* count);
