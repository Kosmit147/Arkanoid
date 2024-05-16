#include "structures.h"
#include <stdio.h>

Quadtree* globalQuadTree = NULL;

Quadtree* createQuadtree(int level, Block bounds)
{
    Quadtree* quadtree = (Quadtree*)malloc(sizeof(Quadtree));
    quadtree->level = level;
    quadtree->bounds = bounds;
    for (int i = 0; i < 4; i++)
    {
        quadtree->nodes[i] = NULL;
    }
    for (int i = 0; i < MAX_OBJECTS; i++)
    {
        quadtree->objects[i] = NULL;
    }
    return quadtree;
}

void split(Quadtree* quadtree)
{
    float subWidth = quadtree->bounds.width / 2;
    float subHeight = quadtree->bounds.height / 2;
    float x = quadtree->bounds.position.x;
    float y = quadtree->bounds.position.y;

    quadtree->nodes[0] = createQuadtree(quadtree->level + 1, (Block) { { .x = x + subWidth, .y = y }, subWidth, subHeight });
    quadtree->nodes[1] = createQuadtree(quadtree->level + 1, (Block) { { .x = x, .y = y }, subWidth, subHeight });
    quadtree->nodes[2] = createQuadtree(quadtree->level + 1, (Block) { { .x = x, .y = y + subHeight }, subWidth, subHeight });
    quadtree->nodes[3] = createQuadtree(quadtree->level + 1, (Block) { { .x = x + subWidth, .y = y + subHeight }, subWidth, subHeight });
}

int getIndex(Block bounds, Block* object)
{
    int index = -1;
    float verticalMidpoint = bounds.position.x + (bounds.width / 2);
    float horizontalMidpoint = bounds.position.y + (bounds.height / 2);

    // Object can completely fit within the top quadrants
    bool topQuadrant = (object->position.y < horizontalMidpoint && object->position.y + object->height < horizontalMidpoint);
    // Object can completely fit within the bottom quadrants
    bool bottomQuadrant = (object->position.y > horizontalMidpoint);

    // Object can completely fit within the left quadrants
    if (object->position.x < verticalMidpoint && object->position.x + object->width < verticalMidpoint)
    {
        if (topQuadrant)
        {
            index = 1;
        }
        else if (bottomQuadrant)
        {
            index = 2;
        }
    }
    // Object can completely fit within the right quadrants
    else if (object->position.x > verticalMidpoint)
    {
        if (topQuadrant)
        {
            index = 0;
        }
        else if (bottomQuadrant)
        {
            index = 3;
        }
    }
    return index;
}

void insert(Quadtree* quadtree, Block* object)
{
    if (quadtree->nodes[0] != NULL)
    {
        int index = getIndex(quadtree->bounds, object);
        if (index != -1)
        {
            insert(quadtree->nodes[index], object);
            return;
        }
    }

    // Jeśli lista obiektów jest pełna, dzielimy obszar na podobszary
    if (quadtree->objects[MAX_OBJECTS - 1] != NULL)
    {
        // Dzielimy obszar na podobszary
        split(quadtree);

        // Przeszukujemy wszystkie obiekty na liście
        for (int i = 0; i < MAX_OBJECTS; i++)
        {
            // Rekurencyjnie wstawiamy klon obiektu do odpowiedniego podobszaru
            int index = getIndex(quadtree->bounds, quadtree->objects[i]);
            if (index != -1)
            {
                Block* object_clone = (Block*)malloc(sizeof(Block));
                *object_clone = *quadtree->objects[i]; // Klonujemy obiekt
                insert(quadtree->nodes[index], object_clone);
            }
            quadtree->objects[i] = NULL; // Czyścimy referencję do obiektu z listy
        }
    }

    // Dodajemy klon nowego obiektu do listy w bieżącym węźle
    for (int i = 0; i < MAX_OBJECTS; i++)
    {
        if (quadtree->objects[i] == NULL)
        {
            Block* object_clone = (Block*)malloc(sizeof(Block));
            *object_clone = *object; // Klonujemy obiekt
            quadtree->objects[i] = object_clone;
            break;
        }
    }
}

void retrieve(Quadtree* quadtree, Block* object)
{
    int index = getIndex(quadtree->bounds, object);
    if (index != -1 && quadtree->nodes[0] != NULL)
    {
        retrieve(quadtree->nodes[index], object);
    }
}

void display(Quadtree* quadtree)
{
    printf("Quadtree bounds: (%.2f, %.2f, %.2f, %.2f)\n",
        (double)quadtree->bounds.position.x, (double)quadtree->bounds.position.y,
        (double)quadtree->bounds.width, (double)quadtree->bounds.height);

    for (int i = 0; i < MAX_OBJECTS; i++)
    {
        if (quadtree->objects[i] != NULL)
        {
            printf("Object at (%.2f, %.2f, %.2f, %.2f)\n",
                (double)quadtree->objects[i]->position.x, (double)quadtree->objects[i]->position.y,
                (double)quadtree->objects[i]->width, (double)quadtree->objects[i]->height);
        }
    }

    for (int i = 0; i < 4; i++)
    {
        if (quadtree->nodes[i] != NULL)
        {
            display(quadtree->nodes[i]);
        }
    }
}

void removeBlock(Quadtree* quadtree, Block* block)
{
    int index = getIndex(quadtree->bounds, block);
    if (index != -1 && quadtree->nodes[0] != NULL)
    {
        removeBlock(quadtree->nodes[index], block);
    }
    else
    {
        for (int i = 0; i < MAX_OBJECTS; i++)
        {
            if (quadtree->objects[i] == block)
            {
                free(quadtree->objects[i]);
                quadtree->objects[i] = NULL;
                return;
            }
        }
    }
}

void retrieveBlocks(Quadtree* quadtree, Vec2 position, Block** blocks, size_t* count)
{
    int index = getIndex(quadtree->bounds, &(Block){position, 0, 0});

    if (index != -1 && quadtree->nodes[0] != NULL)
    {
        retrieveBlocks(quadtree->nodes[index], position, blocks, count);
    }
    else
    {
        for (int i = 0; i < MAX_OBJECTS; i++)
        {
            if (quadtree->objects[i] != NULL)
            {
                blocks[*count] = quadtree->objects[i];
                (*count)++;
            }
        }
    }
}