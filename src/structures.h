#pragma once
#include "entities.h"
#include <stdbool.h>
#define MAX_OBJECTS 10
#define MAX_LEVELS 6


typedef struct Quadtree
{
    int level;
    Block bounds;
    struct Quadtree* nodes[4];
    Block* objects[MAX_OBJECTS];
} Quadtree;

extern Quadtree* globalQuadTree;

Quadtree* createQuadtree(int level, Block bounds);
void split(Quadtree* quadtree);
int getIndex(Block bounds, Block* object);
void insert(Quadtree* quadtree, Block* object);
void retrieve(Quadtree* quadtree, Block* object);
void display(Quadtree* quadtree);
void removeBlock(Quadtree* quadtree, Block* block);
void retrieveBlocks(Quadtree* quadtree, Vec2 position, Block** blocks, size_t* count);

