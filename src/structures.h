#pragma once

#include <stdbool.h>

#include "entities.h"

#define MAX_OBJECTS 10
#define MAX_LEVELS 6
#define INVALID_INDEX SIZE_MAX

#define TOP_LEFT_QUADRANT 0
#define TOP_RIGHT_QUADRANT 1
#define BOTTOM_RIGHT_QUADRANT 2
#define BOTTOM_LEFT_QUADRANT 3

typedef struct QuadTree
{
    int level;
    Rect bounds;
    struct QuadTree* nodes[4];
    Block* objects[MAX_OBJECTS];
    size_t objCount;
} QuadTree;

QuadTree* createQuadTree(int level, Rect bounds);
void split(QuadTree* quadTree);
size_t getIndex(Rect bounds, Block* object);
void insert(QuadTree* quadTree, Block* object);
void retrieve(QuadTree* quadTree, Block* object);
Block* retrieveNth(const QuadTree* quadTree, size_t index);
void display(QuadTree* quadTree);
void removeBlock(QuadTree* quadTree, Block* block);
void retrieveBlocks(QuadTree* quadTree, Vec2 position, Block** blocks, size_t* count);

