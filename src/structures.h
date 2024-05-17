#pragma once
#include "entities.h"
#include <stdbool.h>
#define MAX_OBJECTS 10
#define MAX_LEVELS 6

#define INVALID_INDEX SIZE_MAX

typedef struct Quadtree
{
    int level;
    Rect bounds;
    struct Quadtree* nodes[4];
    Block* objects[MAX_OBJECTS];
    size_t objCount;
} Quadtree;

Quadtree* createQuadtree(int level, Rect bounds);
void split(Quadtree* quadtree);
size_t getIndex(Rect bounds, Block* object);
void insert(Quadtree* quadtree, Block* object);
void retrieve(Quadtree* quadtree, Block* object);
Block* retrieveNth(const Quadtree* quadTree, size_t index);
void display(Quadtree* quadtree);
void removeBlock(Quadtree* quadtree, Block* block);
void retrieveBlocks(Quadtree* quadtree, Vec2 position, Block** blocks, size_t* count);

